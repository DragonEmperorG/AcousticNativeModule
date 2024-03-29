/**
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "AcousticsEngine.h"
#include <cassert>
#include <logging_macros.h>
#include <climits>

/**
 * Duplex is not very stable right after starting up:
 *   the callbacks may not be happening at the right times
 * The time to get it stable varies on different systems. Half second
 * is used for this sample, during the time this sample plays silence.
 */
const float kSystemWarmupTime = 0.5f;

AcousticsEngine::AcousticsEngine() {
//    assert(mOutputChannelCount == mInputChannelCount);
}

AcousticsEngine::~AcousticsEngine() {

    stopStream(mPlayStream);

    stopStream(mRecordingStream);

    closeStream(mPlayStream);

    closeStream(mRecordingStream);
}

void AcousticsEngine::setRecordingDeviceId(int32_t deviceId) {
    mRecordingDeviceId = deviceId;
}

void AcousticsEngine::setPlaybackDeviceId(int32_t deviceId) {
    mPlaybackDeviceId = deviceId;
}

bool AcousticsEngine::isAAudioSupported() {
    oboe::AudioStreamBuilder builder;
    return builder.isAAudioSupported();
}
bool AcousticsEngine::setAudioApi(oboe::AudioApi api) {
    if (mIsEffectOn) return false;

    mAudioApi = api;
    return true;
}
void AcousticsEngine::setEffectOn(bool isOn) {
    if (isOn != mIsEffectOn) {
        mIsEffectOn = isOn;

        if (isOn) {
            openAllStreams();
        } else {
            closeAllStreams();
        }
    }
}

void AcousticsEngine::openAllStreams() {
    // Note: The order of stream creation is important. We create the playback
    // stream first, then use properties from the playback stream
    // (e.g. sample rate) to create the recording stream. By matching the
    // properties we should get the lowest latency path
    openPlaybackStream();
    openRecordingStream();
    // Now start the recording stream first so that we can read from it during
    // the playback stream's dataCallback
    if (mRecordingStream && mPlayStream) {
        startStream(mRecordingStream);
        startStream(mPlayStream);
    } else {
        LOGE("Failed to create recording (%p) and/or playback (%p) stream",
             mRecordingStream, mPlayStream);
        closeAllStreams();
    }
}

/**
 * Stops and closes the playback and recording streams.
 */
void AcousticsEngine::closeAllStreams() {
    /**
     * Note: The order of events is important here.
     * The playback stream must be closed before the recording stream. If the
     * recording stream were to be closed first the playback stream's
     * callback may attempt to read from the recording stream
     * which would cause the app to crash since the recording stream would be
     * null.
     */

    if (mPlayStream != nullptr) {
        closeStream(mPlayStream);  // Calling close will also stop the stream
        mPlayStream = nullptr;
    }

    if (mRecordingStream != nullptr) {
        closeStream(mRecordingStream);
        mRecordingStream = nullptr;
    }
}

/**
 * Creates an audio stream for recording. The audio device used will depend on
 * mRecordingDeviceId.
 * If the value is set to oboe::Unspecified then the default recording device
 * will be used.
 */
void AcousticsEngine::openRecordingStream() {
    // To create a stream we use a stream builder. This allows us to specify all
    // the parameters for the stream prior to opening it
    oboe::AudioStreamBuilder builder;

    setupRecordingStreamParameters(&builder);

    mSoundRecording.setStartRecordingTimeStamp();

    // Now that the parameters are set up we can open the stream
    oboe::Result result = builder.openStream(&mRecordingStream);
    if (result == oboe::Result::OK && mRecordingStream) {

        mFormat = mRecordingStream->getFormat();
        mInputChannelCount = mRecordingStream->getChannelCount();
        mSampleRate = mRecordingStream->getSampleRate();

        LOGV(TAG, "openRecordingStream(): mRecordingDeviceId = ");
        LOGV(TAG, std::to_string(mRecordingDeviceId).c_str());
        LOGV(TAG, "openRecordingStream(): mRecordingDirection = ");
        LOGV(TAG, oboe::convertToText(mRecordingDirection));
        LOGV(TAG, "openRecordingStream(): mSharingMode = ");
        LOGV(TAG, oboe::convertToText(mSharingMode));
        LOGV(TAG, "openRecordingStream(): mFormat = ");
        LOGV(TAG, oboe::convertToText(mFormat));
        LOGV(TAG, "openRecordingStream(): mInputChannelCount = ");
        LOGV(TAG, std::to_string(mInputChannelCount).c_str());
        LOGV(TAG, "openRecordingStream(): mSampleRate = ");
        LOGV(TAG, std::to_string(mSampleRate).c_str());

        warnIfNotLowLatency(mRecordingStream);
    } else {
        LOGE(TAG, "Failed to create recording stream. Error: %s",
             oboe::convertToText(result));
    }
}

/**
 * Creates an audio stream for playback. The audio device used will depend on
 * mPlaybackDeviceId.
 * If the value is set to oboe::Unspecified then the default playback device
 * will be used.
 */
void AcousticsEngine::openPlaybackStream() {
    oboe::AudioStreamBuilder builder;

    setupPlaybackStreamParameters(&builder);
    oboe::Result result = builder.openStream(&mPlayStream);
    if (result == oboe::Result::OK && mPlayStream) {
        mSampleRate = mPlayStream->getSampleRate();

        assert(mPlayStream->getFormat() == oboe::AudioFormat::I16);
        assert(mOutputChannelCount == mPlayStream->getChannelCount());

        mSystemStartupFrames =
            static_cast<uint64_t>(mSampleRate * kSystemWarmupTime);
        mProcessedFrameCount = 0;

        warnIfNotLowLatency(mPlayStream);

    } else {
        LOGE("Failed to create playback stream. Error: %s",
             oboe::convertToText(result));
    }
}

/**
 * Sets the stream parameters which are specific to recording,
 * including the sample rate which is determined from the
 * playback stream.
 *
 * @param builder The recording stream builder
 */
oboe::AudioStreamBuilder *AcousticsEngine::setupRecordingStreamParameters(
    oboe::AudioStreamBuilder *builder) {
    //
    builder->setCallback(&recordingCallback)
        ->setDeviceId(mRecordingDeviceId)
        ->setDirection(mRecordingDirection)
        ->setSampleRate(mSampleRate)
        ->setChannelCount(mInputChannelCount);
    return setupCommonStreamParameters(builder);
}

/**
 * Sets the stream parameters which are specific to playback, including device
 * id and the dataCallback function, which must be set for low latency
 * playback.
 * @param builder The playback stream builder
 */
oboe::AudioStreamBuilder *AcousticsEngine::setupPlaybackStreamParameters(
    oboe::AudioStreamBuilder *builder) {
    builder->setCallback(this)
        ->setDeviceId(mPlaybackDeviceId)
        ->setDirection(oboe::Direction::Output)
        ->setChannelCount(mOutputChannelCount);

    return setupCommonStreamParameters(builder);
}

/**
 * Set the stream parameters which are common to both recording and playback
 * streams.
 * @param builder The playback or recording stream builder
 */
oboe::AudioStreamBuilder *AcousticsEngine::setupCommonStreamParameters(
    oboe::AudioStreamBuilder *builder) {
    // We request EXCLUSIVE mode since this will give us the lowest possible
    // latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED
    // mode.
    builder->setSharingMode(mSharingMode)
        ->setFormat(mFormat)
        ->setPerformanceMode(mPerformanceMode)
        ->setAudioApi(mAudioApi);
    return builder;
}

void AcousticsEngine::startStream(oboe::AudioStream *stream) {
    assert(stream);
    if (stream) {
        oboe::Result result = stream->requestStart();
        if (result != oboe::Result::OK) {
            LOGE("Error starting stream. %s", oboe::convertToText(result));
        }
    }
}

void AcousticsEngine::stopStream(oboe::AudioStream *stream) {

    LOGD(TAG, "stopStream(): ");

    if (stream) {
        oboe::Result result = stream->stop(0L);
        if (result != oboe::Result::OK) {
            LOGE("Error stopping stream. %s", oboe::convertToText(result));
        }
        LOGW(TAG, "stopStream(): mTotalSamples = ");
        LOGW(TAG, std::to_string(mSoundRecording.getTotalSamples()).c_str());
    }
}

/**
 * Close the stream. AudioStream::close() is a blocking call so
 * the application does not need to add synchronization between
 * onAudioReady() function and the thread calling close().
 * [the closing thread is the UI thread in this sample].
 * @param stream the stream to close
 */
void AcousticsEngine::closeStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->close();
        if (result != oboe::Result::OK) {
            LOGE("Error closing stream. %s", oboe::convertToText(result));
        }
    }
}

/**
 * Restart the streams. During the restart operation subsequent calls to this
 * method will output a warning.
 */
void AcousticsEngine::restartStreams() {
    LOGI("Restarting streams");

    if (mRestartingLock.try_lock()) {
        closeAllStreams();
        openAllStreams();
        mRestartingLock.unlock();
    } else {
        LOGW(
            "Restart stream operation already in progress - ignoring this "
            "request");
        // We were unable to obtain the restarting lock which means the restart
        // operation is currently
        // active. This is probably because we received successive "stream
        // disconnected" events.
        // Internal issue b/63087953
    }
}

/**
 * Warn in logcat if non-low latency stream is created
 * @param stream: newly created stream
 *
 */
void AcousticsEngine::warnIfNotLowLatency(oboe::AudioStream *stream) {
    if (stream->getPerformanceMode() != oboe::PerformanceMode::LowLatency) {
        LOGW(
            "Stream is NOT low latency."
            "Check your requested format, sample rate and channel count");
    }
}

/**
 * Handles playback stream's audio request. In this sample, we simply block-read
 * from the record stream for the required samples.
 *
 * @param oboeStream: the playback stream that requesting additional samples
 * @param audioData:  the buffer to load audio samples for playback stream
 * @param numFrames:  number of frames to load to audioData buffer
 * @return: DataCallbackResult::Continue.
 */
oboe::DataCallbackResult AcousticsEngine::onAudioReady(
    oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) {
    assert(oboeStream == mPlayStream);

    int32_t prevFrameRead = 0, framesRead = 0;
    if (mProcessedFrameCount < mSystemStartupFrames) {
        do {
            // Drain the audio for the starting up period, half second for
            // this sample.
            prevFrameRead = framesRead;

            oboe::ResultWithValue<int32_t> status =
                mRecordingStream->read(audioData, numFrames, 0);
            framesRead = (!status) ? 0 : status.value();
            if (framesRead == 0) break;

        } while (framesRead);

        framesRead = prevFrameRead;
    } else {
        oboe::ResultWithValue<int32_t> status =
            mRecordingStream->read(audioData, numFrames, 0);
        if (!status) {
            LOGE("input stream read error: %s",
                 oboe::convertToText(status.error()));
            return oboe::DataCallbackResult ::Stop;
        }
        framesRead = status.value();
    }

    if (framesRead < numFrames) {
        int32_t bytesPerFrame = mRecordingStream->getChannelCount() *
                                oboeStream->getBytesPerSample();
        uint8_t *padPos =
            static_cast<uint8_t *>(audioData) + framesRead * bytesPerFrame;
        memset(padPos, 0, static_cast<size_t>((numFrames - framesRead) * bytesPerFrame));
    }

    // add your audio processing here

    mProcessedFrameCount += numFrames;

    return oboe::DataCallbackResult::Continue;
}

/**
 * Oboe notifies the application for "about to close the stream".
 *
 * @param oboeStream: the stream to close
 * @param error: oboe's reason for closing the stream
 */
void AcousticsEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream,
                                          oboe::Result error) {
    LOGE("%s stream Error before close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}

/**
 * Oboe notifies application that "the stream is closed"
 *
 * @param oboeStream
 * @param error
 */
void AcousticsEngine::onErrorAfterClose(oboe::AudioStream *oboeStream,
                                         oboe::Result error) {
    LOGE("%s stream Error after close: %s",
         oboe::convertToText(oboeStream->getDirection()),
         oboe::convertToText(error));
}

void AcousticsEngine::initialRecordAudio() {

    LOGD(TAG, "initialRecordAudio(): ");

    openRecordingStream();
    mSoundRecording.initialSignalDetectionConfig(mFormat, mInputChannelCount, mSampleRate);
    startRecordAudio();

}

/**
 * Start Recording Audio
 *
 * @param
 * @param
 */
void AcousticsEngine::startRecordAudio() {

    LOGD(TAG, "startRecording(): ");

    if (mRecordingStream) {
        startStream(mRecordingStream);
    } else {
        LOGE(TAG, "startRecording(): Failed to create recording (%p) stream", mRecordingStream);
        closeRecordingStream();
    }

//    constexpr int kMillisecondsToRecord = 2;
//    const int32_t requestedFrame = (int32_t)(kMillisecondsToRecord
//            * (mRecordingStream->getSampleRate() / oboe::kMillisPerSecond));
//    int16_t recorderBuffer[requestedFrame];
//
//    constexpr int64_t kTimeoutValue = 3 * oboe::kNanosPerMillisecond;
//
//    int framesRead = 0;
//    do {
//        auto result = mRecordingStream->read(recorderBuffer,
//                mRecordingStream->getBufferSizeInFrames(), 0);
//        if (result != oboe::Result::OK) break;
//        framesRead = result.value();
//    } while (framesRead != 0);
//
//    while (mIsRecordAudioOn) {
//        auto result = mRecordingStream->read(recorderBuffer, mRecordingStream->getBufferSizeInFrames(),
//                                             kTimeoutValue);
//        if (result != oboe::Result::OK) {
//            LOGD("Read %d frames", result.value());
//        } else {
//            LOGE("Error reading stream: %s", oboe::convertToText(result.error()));
//        }
//    }
}

/**
 * Stop Recording Audio
 *
 * @param
 * @param
 */
void AcousticsEngine::stopRecordAudio() {

    LOGD(TAG, "stopRecordAudio(): ");

    stopStream(mRecordingStream);
}

/**
 * Close Recording Audio
 */
void AcousticsEngine::closeRecordAudio() {

    LOGD(TAG, "closeRecordAudio(): ");

    stopStream(mRecordingStream);

    closeRecordingStream();

    mRecordingStream = nullptr;

    mSoundRecording.clear();
}

/**
 * Close Recording Audio
 */
void AcousticsEngine::saveRecordAudio(const char* filePath) {

    LOGD(TAG, "saveRecordAudio(): ");

    mSoundRecording.initiateWritingToFile(filePath, mFormat, mInputChannelCount, mSampleRate);
}

/**
 * Close recording stream.
 * @param stream the stream to close
 */
void AcousticsEngine::closeRecordingStream() {

    LOGD(TAG, "closeRecordingStream(): ");

    if (mRecordingStream != nullptr) {
        closeStream(mRecordingStream);
        mRecordingStream = nullptr;
    }

}

/**
 * Read record audio wave Data stored in memory to paint.
 * @param stream the stream to close
 */
int32_t AcousticsEngine::readPaintRecordAudioWaveBuffer(float *audioData, int32_t offsetInShorts, int32_t sizeInShorts) {

    LOGD(TAG, "readPaintRecordAudioWaveBuffer(): ");

    return mSoundRecording.readMData2Paint(audioData, offsetInShorts, sizeInShorts);

}
