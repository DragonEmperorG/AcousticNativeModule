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

#include <jni.h>
#include <logging_macros.h>
#include "AcousticsEngine.h"

static const int kOboeApiAAudio = 0;
static const int kOboeApiOpenSLES = 1;

const char *TAG = "jni_bridge:: %s";
static AcousticsEngine *engine = nullptr;

extern "C" {

JNIEXPORT bool JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_create(JNIEnv *env,
                                                           jclass) {
    LOGD("create AcousticsEngine");

    if (engine == nullptr) {
        engine = new AcousticsEngine();
    }

    return (engine != nullptr);
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_delete(JNIEnv *env,
                                                           jclass) {
    delete engine;
    engine = nullptr;
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_setEffectOn(
        JNIEnv *env, jclass, jboolean isEffectOn) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setEffectOn(isEffectOn);
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_setRecordingDeviceId(
        JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setRecordingDeviceId(deviceId);
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_setPlaybackDeviceId(
        JNIEnv *env, jclass, jint deviceId) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    engine->setPlaybackDeviceId(deviceId);
}

JNIEXPORT jboolean JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_setAPI(JNIEnv *env,
                                                           jclass type,
                                                           jint apiType) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine "
                "before calling this method");
        return JNI_FALSE;
    }

    oboe::AudioApi audioApi;
    switch (apiType) {
        case kOboeApiAAudio:
            audioApi = oboe::AudioApi::AAudio;
            break;
        case kOboeApiOpenSLES:
            audioApi = oboe::AudioApi::OpenSLES;
            break;
        default:
            LOGE("Unknown API selection to setAPI() %d", apiType);
            return JNI_FALSE;
    }

    return static_cast<jboolean>(engine->setAudioApi(audioApi) ? JNI_TRUE
                                                               : JNI_FALSE);
}

JNIEXPORT jboolean JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_isAAudioSupported(
        JNIEnv *env, jclass type) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine "
                "before calling this method");
        return JNI_FALSE;
    }
    return static_cast<jboolean>(engine->isAAudioSupported() ? JNI_TRUE
                                                             : JNI_FALSE);
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_initialRecordAudio(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    LOGD("engine->setRecordAudioOn(isRecordAudioOn);");
    engine->initialRecordAudio();
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_startRecordAudio(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    LOGD("engine->setRecordAudioOn(isRecordAudioOn);");
    engine->startRecordAudio();
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_pauseRecordAudio(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    LOGD("engine->setRecordAudioOn(isRecordAudioOn);");
    engine->stopRecordAudio();
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_stopRecordAudio(
        JNIEnv *env, jclass) {
    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    LOGD("engine->setRecordAudioOn(isRecordAudioOn);");
    engine->closeRecordAudio();
}

JNIEXPORT void JNICALL
Java_cn_edu_whu_lmars_acousticlocsdk_AcousticsEngine_saveRecordAudio(
        JNIEnv *env, jclass, jstring filePath) {
    LOGD(TAG, "writeFile(): filePath = ");
    LOGD(TAG, filePath);

    if (engine == nullptr) {
        LOGE(
                "Engine is null, you must call createEngine before calling this "
                "method");
        return;
    }

    const char *path;
    path = env->GetStringUTFChars(filePath, nullptr);
    engine->saveRecordAudio(path);
    env->ReleaseStringUTFChars(filePath, path);
}
}
