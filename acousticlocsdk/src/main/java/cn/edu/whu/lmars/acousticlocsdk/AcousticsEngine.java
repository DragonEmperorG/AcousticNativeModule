package cn.edu.whu.lmars.acousticlocsdk;

public enum AcousticsEngine {

    INSTANCE;

    // Load native library
    static {
        System.loadLibrary("acousticlocsdk");
    }

    // Native methods
    static native boolean create();
    static native boolean isAAudioSupported();
    static native boolean setAPI(int apiType);
    static native void setEffectOn(boolean isEffectOn);
    static native void setRecordingDeviceId(int deviceId);
    static native void setPlaybackDeviceId(int deviceId);
    static native void delete();
}
