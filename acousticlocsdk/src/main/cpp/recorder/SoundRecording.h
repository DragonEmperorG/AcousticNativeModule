
#ifndef ACOUSTICNATIVEMODULE_SOUNDRECORDING_H
#define ACOUSTICNATIVEMODULE_SOUNDRECORDING_H

#ifndef MODULE_NAME
#define MODULE_NAME  "SoundRecording"
#endif

#include <atomic>
#include <sndfile.hh>
#include <time.h>
#include <vector>

#include <oboe/Definitions.h>
#include "Utils.h"


constexpr int kMaxSamples = 48000 * 60; // 60s of audio data @ 48kHz

class SoundRecording {

public:
    int32_t write(const int16_t *sourceData, int32_t numSamples);
    int32_t writeFormatI16(const int16_t *sourceData, int32_t numSamples);
    int32_t writeFormatFloat(const float_t *sourceData, int32_t numSamples);
    int32_t read(int16_t *targetData, int32_t numSamples);
    int32_t readFormatI16(int16_t *targetData, int32_t numSamples);
    int32_t readFormatFloat(float_t *targetData, int32_t numSamples);

    int16_t* readMData2Paint(int offsetInShorts, int sizeInShorts);

    SndfileHandle createFile(const char *outfilename, oboe::AudioFormat sampleFormat, int32_t outputChannels, int32_t sampleRate);

    void initiateWritingToFile(const char *outfilename, oboe::AudioFormat sampleFormat, int32_t sampleChannels, int32_t sampleRate);
    void writeFile(SndfileHandle sndfileHandle, oboe::AudioFormat sampleFormat);
    void readFileInfo(const char * fileName);
    void initialSignalDetectionConfig(oboe::AudioFormat sampleFormat, int32_t sampleChannels, int32_t sampleRate);
    void setStartRecordingTimeStamp();

    bool updateSlideWindow();

    bool isFull() const {
        return (mWriteIndex == kMaxSamples);
    };

    void clear() {
        mWriteIndex = 0;
        mPaintIndex = 0;
        mDataFormatI16 = new int16_t[kMaxSamples]{0};
        mSlideWindowIndex = 0;
        mTotalSamples = 0;
    };

    void setLooping(bool isLooping) {
        mIsLooping = isLooping;
    };

    void setReadPositionToStart() {
        mReadIndex = 0;
    };

    int32_t getLength() const {
        return mWriteIndex;
    };

    int32_t getTotalSamples() const {
        return mTotalSamples;
    };

    static const int32_t getMaxSamples() {
        return kMaxSamples;
    };

private:
    const char* TAG = "SoundRecording:: %s";

    std::atomic<int32_t> mIteration { 1 };
    std::atomic<int32_t> mWriteIndex { 0 };
    std::atomic<int32_t> mReadIndex { 0 };
    std::atomic<int32_t> mPaintIndex { 0 };
    std::atomic<int32_t> mSlideWindowIndex { 0 };
    std::atomic<int32_t> mTotalSamples { 0 };
    std::atomic<bool> mIsLooping { false };

    int16_t* mDataFormatI16 = new int16_t[kMaxSamples]{0};
    float_t* mDataFormatFloat = new float_t[kMaxSamples]{0};

    // Signal Detection Slide Window Format Float
    float_t* mSigDetectSlideWindowFF;
    // Signal Detection Slide Window Time Interval(ms)
    int32_t mSlideWindowTimeInterval = 50;
    int32_t mSlideWindowBufferLength;
    // Start Recording Timestamp(μs)
    long mStartRecordingTimeStamp = 0;

    // 6 Decibels gain on audio signal
    int16_t mGainFactor = 1;
};

#endif //ACOUSTICNATIVEMODULE_SOUNDRECORDING_H
