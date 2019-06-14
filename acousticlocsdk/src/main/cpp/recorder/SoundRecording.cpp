#include "SoundRecording.h"

int32_t SoundRecording::write(const int16_t *sourceData, int32_t numSamples) {

    LOGD(TAG, "write(): ");

    // Check that data will fit, if it doesn't then create new array,
    // copy over old data to new array then delete old array and
    // point mDataFormatI16 to new array.
    if (mWriteIndex + numSamples > (kMaxSamples * mIteration)) {

        LOGW(TAG, "write(): mWriteIndex + numSamples > kMaxSamples");
        mIteration++;

        int32_t newSize = kMaxSamples * mIteration;

        auto * newData = new int16_t[newSize] { 0 };
        std::copy(mDataFormatI16, mDataFormatI16 + mTotalSamples, newData);

        delete[] mDataFormatI16;
        mDataFormatI16 = newData;

    }

    for (int i = 0; i < numSamples; ++i) {
        mDataFormatI16[mWriteIndex++] = sourceData[i] * mGainFactor;
    }

    mTotalSamples += numSamples;

    return numSamples;
}


int32_t SoundRecording::writeFormatI16(const int16_t *sourceData, int32_t numSamples) {

    LOGD(TAG, "writeFormatI16(): ");

    // Check that data will fit, if it doesn't then create new array,
    // copy over old data to new array then delete old array and
    // point mDataFormatI16 to new array.
    if (mWriteIndex + numSamples > (kMaxSamples * mIteration)) {

        LOGW(TAG, "write(): mWriteIndex + numSamples > kMaxSamples");
        mIteration++;

        int32_t newSize = kMaxSamples * mIteration;

        auto * newData = new int16_t[newSize] { 0 };
        std::copy(mDataFormatI16, mDataFormatI16 + mTotalSamples, newData);

        delete[] mDataFormatI16;
        mDataFormatI16 = newData;

    }

    for (int i = 0; i < numSamples; ++i) {
        mDataFormatI16[mWriteIndex++] = sourceData[i] * mGainFactor;
    }

    mTotalSamples += numSamples;

    return numSamples;
}

int32_t SoundRecording::writeFormatFloat(const float_t *sourceData, int32_t numSamples) {

    LOGD(TAG, "writeFormatFloat(): ");

    // Check that data will fit, if it doesn't then create new array,
    // copy over old data to new array then delete old array and
    // point mDataFormatI16 to new array.
    if (mWriteIndex + numSamples > (kMaxSamples * mIteration)) {

        LOGW(TAG, "write(): mWriteIndex + numSamples > kMaxSamples");
        mIteration++;

        int32_t newSize = kMaxSamples * mIteration;

        auto * newData = new float_t[newSize] { 0 };
        std::copy(mDataFormatFloat, mDataFormatFloat + mTotalSamples, newData);

        delete[] mDataFormatFloat;
        mDataFormatFloat = newData;

    }

    for (int i = 0; i < numSamples; ++i) {
//        LOGD(TAG, std::to_string(sourceData[i]).c_str());
        mDataFormatFloat[mWriteIndex++] = sourceData[i] * mGainFactor;
    }

    mTotalSamples += numSamples;

    return numSamples;
}


int32_t SoundRecording::read(int16_t *targetData, int32_t numSamples) {

    LOGD(TAG, "read(): ");

    int32_t framesRead = 0;

    while (framesRead < numSamples && mReadIndex < mTotalSamples) {
        targetData[framesRead++] = mDataFormatI16[mReadIndex++];
        if (mIsLooping && mReadIndex == mTotalSamples) mReadIndex = 0;
    }

    return framesRead;
}

int32_t SoundRecording::readFormatI16(int16_t *targetData, int32_t numSamples) {

    LOGD(TAG, "readFormatI16(): ");

    int32_t framesRead = 0;

    while (framesRead < numSamples && mReadIndex < mTotalSamples) {
        targetData[framesRead++] = mDataFormatI16[mReadIndex++];
        if (mIsLooping && mReadIndex == mTotalSamples) mReadIndex = 0;
    }

    return framesRead;
}
int32_t SoundRecording::readFormatFloat(float_t *targetData, int32_t numSamples) {

    LOGD(TAG, "readFormatFloat(): ");

    int32_t framesRead = 0;

    while (framesRead < numSamples && mReadIndex < mTotalSamples) {
        targetData[framesRead++] = mDataFormatFloat[mReadIndex++];
        if (mIsLooping && mReadIndex == mTotalSamples) mReadIndex = 0;
    }

    return framesRead;
}

SndfileHandle SoundRecording::createFile(const char *outfilename, oboe::AudioFormat sampleFormat, int32_t sampleChannels, int32_t sampleRate) {

    LOGD(TAG, "createFile(): ");

    SndfileHandle file;
    int format = SF_FORMAT_WAV;

    if (sampleFormat == oboe::AudioFormat::I16) {
        format = format | SF_FORMAT_PCM_16;
    } else if (sampleFormat == oboe::AudioFormat::Float) {
        format = format | SF_FORMAT_FLOAT;
    }

    LOGD(TAG, "createFile(): Creating file ");

    file = SndfileHandle(outfilename, SFM_WRITE, format, sampleChannels, sampleRate);

    return file;
}

void SoundRecording::writeFile(SndfileHandle sndfileHandle, oboe::AudioFormat sampleFormat) {

    LOGD(TAG, "writeFile(): ");

    if (sampleFormat == oboe::AudioFormat::I16) {
        int32_t framesRead = 0, bufferLength = mTotalSamples / 3;
        sf_count_t framesWrite = 0;

        auto *buffer = new int16_t[bufferLength];

        fillArrayWithZeros(buffer, bufferLength);

        while ((framesRead = read(buffer, bufferLength)) > 0) {
            framesWrite = sndfileHandle.write(buffer, framesRead);
        }
    } else if (sampleFormat == oboe::AudioFormat::Float) {
        int32_t framesRead = 0, bufferLength = mTotalSamples / 3;
        LOGI(TAG, "writeFile(): bufferLength = ");
        LOGI(TAG, to_string_with_precision(bufferLength).c_str());
        auto *buffer = new float_t[bufferLength];

        fillArrayWithZeros(buffer, bufferLength);

        sf_count_t framesWrite = 0;
        while ((framesRead = readFormatFloat(buffer, bufferLength)) > 0) {
            framesWrite = sndfileHandle.write(buffer, framesRead);
        }
    }

}

void SoundRecording::readFileInfo(const char * fileName) {

    LOGD(TAG, "readFileInfo(): ");

    SndfileHandle file;
    file = SndfileHandle(fileName) ;

    LOGD (TAG, "readFileInfo(): Opened file => ") ;
    LOGD(TAG, fileName);

    LOGI (TAG, "readFileInfo(): Sample rate : ", file.samplerate());
    LOGI(TAG, std::to_string(file.samplerate()).c_str());
    LOGI (TAG, "readFileInfo(): Channels : ", file.channels()) ;
    LOGI(TAG, std::to_string(file.channels()).c_str());
    LOGI (TAG, "readFileInfo(): Frames : ", file.frames()) ;
    LOGI(TAG, std::to_string(file.frames()).c_str());

}

void SoundRecording::initiateWritingToFile(const char *outfilename, oboe::AudioFormat sampleFormat, int32_t sampleChannels, int32_t sampleRate) {

    LOGD(TAG, "initiateWritingToFile(): ");

    SndfileHandle handle = createFile(outfilename, sampleFormat, sampleChannels, sampleRate);
    writeFile(handle, sampleFormat);
    readFileInfo(outfilename);

}



//void SoundRecording::writeToFile(const char *outfilename, int32_t outputChannels, int32_t sampleRate) {
//
//    LOGD(TAG, "writeToFile(): ");
//
//    SNDFILE *outfile, *recordedFile;
//    SF_INFO sfinfo;
//    sf_count_t framesWrite = 0;
////    int32_t framesRead = 0, bufferLength = outputChannels * sampleRate;
//    int32_t framesRead = 0, bufferLength = mTotalSamples;
//    auto* buffer = new int[bufferLength]{0};
//
//    memset(&sfinfo, 0, sizeof(sfinfo));
//
//    sfinfo.frames = mTotalSamples;
//    sfinfo.channels = outputChannels;
//    sfinfo.samplerate = sampleRate;
//    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
//
//
//    LOGV(TAG, "writeToFile(): ==== BEFORE WRITING INPUT INFO ==== ");
//
//    LOGV(TAG, "writeToFile(): sfinfo.frames = ");
//    LOGV(TAG, std::to_string(sfinfo.frames).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.channels = ");
//    LOGV(TAG, std::to_string(sfinfo.channels).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.format = ");
//    LOGV(TAG, std::to_string(sfinfo.format).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.samplerate = ");
//    LOGV(TAG, std::to_string(sfinfo.samplerate).c_str());
//
//    LOGV(TAG, "writeToFile(): ==== BEFORE WRITING INPUT INFO ==== ");
//
//
//    if (!(outfile = sf_open(outfilename, SFM_WRITE, &sfinfo))) {
//        LOGD(TAG, "Error : could not open file : %s\n", outfilename);
//        LOGD(TAG, sf_strerror(outfile));
//        exit(1);
//    };
//
//    if (!sf_format_check(&sfinfo)) {
//        sf_close(outfile);
//        LOGD(TAG, "writeToFile(): Invalid encoding");
//        return;
//    };
//
//    while ((framesRead = read(buffer, bufferLength)) > 0) {
//
//        LOGD(TAG, "writeToFile(): framesRead = ");
//        LOGD(TAG, std::to_string(framesRead).c_str());
//
//        framesWrite = sf_write_int(outfile, buffer, framesRead);
//
//        LOGD(TAG, "writeToFile(): framesWrite = ");
//        LOGD(TAG, std::to_string(framesWrite).c_str());
//
//    }
//
//    sf_close(outfile);
//
//    memset(&sfinfo, 0, sizeof(sfinfo));
//
//    if (!(recordedFile = sf_open(outfilename, SFM_READ, &sfinfo))) {
//        LOGD(TAG, "Error : could not open recordedFile file ");
//        LOGD(TAG, sf_strerror(recordedFile));
//        exit(1);
//    };
//
//    LOGV(TAG, "writeToFile(): ==== RECORDED FILE INFO ==== ");
//
//    LOGV(TAG, "writeToFile(): sfinfo.frames = ");
//    LOGV(TAG, std::to_string(sfinfo.frames).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.channels = ");
//    LOGV(TAG, std::to_string(sfinfo.channels).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.format = ");
//    LOGV(TAG, std::to_string(sfinfo.format).c_str());
//
//    LOGV(TAG, "writeToFile(): sfinfo.samplerate = ");
//    LOGV(TAG, std::to_string(sfinfo.samplerate).c_str());
//
//    LOGV(TAG, "writeToFile(): ==== RECORDED FILE INFO ==== ");
//
//    sf_close(recordedFile);
//
//    LOGD(TAG, "writeToFile(): Done with writing file");
//
//}


void SoundRecording::initialSignalDetectionConfig(oboe::AudioFormat sampleFormat, int32_t sampleChannels, int32_t sampleRate) {
    if (sampleFormat == oboe::AudioFormat::I16) {
        ;
    } else if (sampleFormat == oboe::AudioFormat::Float) {
        mSlideWindowBufferLength = sampleRate * sampleChannels * mSlideWindowTimeInterval / 1000;
        LOGI(TAG, "mSlideWindowBufferLength = ");
        LOGI(TAG, std::to_string(mSlideWindowBufferLength).c_str());
        mSigDetectSlideWindowFF = new float_t[mSlideWindowBufferLength]{ 0 };
    }
}

void SoundRecording::setStartRecordingTimeStamp() {
    struct timeval xTime;
    gettimeofday(&xTime, NULL);
    auto tv_sec = (long long)xTime.tv_sec;
    auto tv_usec = (long long)xTime.tv_usec;
    mStartRecordingTimeStamp = tv_sec * 1000000 + tv_usec;
    LOGI(TAG, "mStartRecordingTimeStamp = ");
    LOGI(TAG, std::to_string(mStartRecordingTimeStamp).c_str());

//    std::string year = std::to_string(xTime.);
}

bool SoundRecording::updateSlideWindow() {

    bool isUpdate = false;

    if (mTotalSamples - mSlideWindowIndex >= 2 * mSlideWindowBufferLength) {
        while (mTotalSamples - mSlideWindowIndex >= mSlideWindowBufferLength) {
            for (int i = 0; i < mSlideWindowBufferLength; ++i) {
                mSigDetectSlideWindowFF[i] = mDataFormatFloat[mSlideWindowIndex + mSlideWindowBufferLength];
                mSlideWindowIndex++;
            }
            isUpdate = true;
            LOGD(TAG, "Updated Slide Window");
        }
    }

    return isUpdate;
}


int32_t SoundRecording::readMData2Paint(float *audioData, int32_t offsetInShorts, int32_t sizeInShorts) {

    LOGI(TAG, "readMData2Paint(): ");

    int32_t numAudioData = 0;

    if (mPaintIndex + sizeInShorts < mTotalSamples) {
        for (int i = 0; i < sizeInShorts; ++i) {
            audioData[i] = mDataFormatFloat[mPaintIndex++];
            numAudioData++;
        }
    }

    LOGI(TAG, "mTotalSamples= ");
    LOGI(TAG, std::to_string(mTotalSamples).c_str());
    LOGI(TAG, "mPaintIndex= ");
    LOGI(TAG, std::to_string(mPaintIndex).c_str());
    LOGI(TAG, "numAudioData= ");
    LOGI(TAG, std::to_string(numAudioData).c_str());

    return numAudioData;
}