
#include "RecordingCallback.h"

oboe::DataCallbackResult
RecordingCallback::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {
    const int32_t numSamples = numFrames * audioStream->getChannelCount();

    if (audioStream->getFormat() == oboe::AudioFormat::I16)
        return processFormatI16RecordingFrames(audioStream, static_cast<int16_t *>(audioData), numSamples);
    else if (audioStream->getFormat() == oboe::AudioFormat::Float)
            return processFormatFloatRecordingFrames(audioStream, static_cast<float_t *>(audioData), numSamples);

    return oboe::DataCallbackResult::Continue;
}

oboe::DataCallbackResult
RecordingCallback::processFormatI16RecordingFrames(oboe::AudioStream *audioStream, int16_t *audioData, int32_t numSamples) {

    LOGD(TAG, "processFormatI16RecordingFrames(): ");

    int32_t framesWritten = mSoundRecording->writeFormatI16(audioData, numSamples);
    LOGD(TAG, "numSamples = ");
    LOGD(TAG, std::to_string(framesWritten).c_str());

    return oboe::DataCallbackResult::Continue;

}

oboe::DataCallbackResult
RecordingCallback::processFormatFloatRecordingFrames(oboe::AudioStream *audioStream, float_t *audioData, int32_t numSamples) {

//    LOGD(TAG, "processFormatFloatRecordingFrames(): ");

    mSoundRecording->writeFormatFloat(audioData, numSamples);
//    int32_t framesWritten = mSoundRecording->writeFormatFloat(audioData, numSamples);
//    LOGD(TAG, "numSamples = ");
//    LOGD(TAG, std::to_string(framesWritten).c_str());

    mSoundRecording->updateSlideWindow();
//    bool isUpdateSlideWindow = mSoundRecording->updateSlideWindow();
//    if (isUpdateSlideWindow)
//        LOGD(TAG, "isUpdateSlideWindow = true");
//    else
//        LOGD(TAG, "isUpdateSlideWindow = false");

    return oboe::DataCallbackResult::Continue;

}
