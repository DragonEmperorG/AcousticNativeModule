/*
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

#ifndef ACOUSTICNATIVEMODULE_ACOUSTICSENGINE_H
#define ACOUSTICNATIVEMODULE_ACOUSTICSENGINE_H

#include <jni.h>
#include <oboe/Oboe.h>
#include <string>
#include <thread>

#include "recorder/RecordingCallback.h"
#include "recorder/SoundRecording.h"

class AcousticsEngine : public oboe::AudioStreamCallback {

   public:
    AcousticsEngine();
    ~AcousticsEngine();

    RecordingCallback recordingCallback = RecordingCallback(&mSoundRecording);

    void setRecordingDeviceId(int32_t deviceId);
    void setPlaybackDeviceId(int32_t deviceId);
    void setEffectOn(bool isOn);
    void setRecordAudioOn(bool isRecordAudioOn);


    /*
     * oboe::AudioStreamCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);
    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    bool setAudioApi(oboe::AudioApi);
    bool isAAudioSupported(void);

   private:
    const char* TAG = "AcousticsEngine:: %s";

    bool mIsEffectOn = false;
    bool mIsRecordAudioOn = false;

    /*
     * Audio Streams
     */
    oboe::AudioStream *mRecordingStream = nullptr;
    oboe::AudioStream *mPlayStream = nullptr;

    /*
     * Audio Device
     * An audio device is a hardware interface or virtual endpoint that acts as a source or sink
     * for a continuous stream of digital audio data.
     *
     * InputPreset
     * VoiceRecognition    Use this preset when doing speech recognition.
     */
    int32_t mRecordingDeviceId = oboe::kUnspecified;
    int32_t mPlaybackDeviceId = oboe::VoiceRecognition;
    /*
     * The audio device attached to a stream determines whether the stream is for input or output.
     * A stream can only move data in one direction. When you define a stream you also set its
     * direction. When you open a stream Android checks to ensure that the audio device and stream
     * direction agree.
     */
    oboe::Direction mRecordingDirection = oboe::Direction::Input;

    /*
     * Sharing Mode
     * Exclusive | Shared
     */
    oboe::SharingMode mSharingMode = oboe::SharingMode::Exclusive;

    /*
     * Audio Format
     * Digital Audio Attributes
     */
    /*
     * Sample Format
     */
    oboe::AudioFormat mFormat = oboe::AudioFormat::I16;
    /*
     * Samples Per Frame
     */
    int32_t mInputChannelCount = oboe::ChannelCount::Stereo;
    int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;
    /*
     * Sample Rate
     */
    int32_t mSampleRate = oboe::kUnspecified;

    uint64_t mProcessedFrameCount = 0;
    uint64_t mSystemStartupFrames = 0;

    /*
     * Performance Mode
     */
    oboe::PerformanceMode mPerformanceMode = oboe::PerformanceMode::LowLatency;

    /*
     * Audio API
     */
    oboe::AudioApi mAudioApi = oboe::AudioApi::AAudio;


    SoundRecording mSoundRecording;
    SndfileHandle sndfileHandle;

    std::mutex mRestartingLock;

    void startRecordAudio();
    void stopRecordAudio();

    void openRecordingStream();
    void closeRecordingStream();

    void openPlaybackStream();

    void startStream(oboe::AudioStream *stream);
    void stopStream(oboe::AudioStream *stream);
    void closeStream(oboe::AudioStream *stream);

    void openAllStreams();
    void closeAllStreams();
    void restartStreams();

    oboe::AudioStreamBuilder *setupCommonStreamParameters(
        oboe::AudioStreamBuilder *builder);
    oboe::AudioStreamBuilder *setupRecordingStreamParameters(
        oboe::AudioStreamBuilder *builder);
    oboe::AudioStreamBuilder *setupPlaybackStreamParameters(
        oboe::AudioStreamBuilder *builder);
    void warnIfNotLowLatency(oboe::AudioStream *stream);
};

#endif  // ACOUSTICNATIVEMODULE_ACOUSTICSENGINE_H
