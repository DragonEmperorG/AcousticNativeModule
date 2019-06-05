package cn.edu.whu.lmars.acousticlocsdk;

public enum AcousticsEngine {

    INSTANCE;

    // Load native library
    static {
        System.loadLibrary("acousticlocsdk");
    }

    // Native methods
    public static native boolean create();
    public static native boolean isAAudioSupported();
    public static native boolean setAPI(int apiType);
    public static native void setEffectOn(boolean isEffectOn);
    public static native void setRecordAudioOn(boolean isRecordAudioOn);
    public static native void setRecordingDeviceId(int deviceId);
    public static native void setPlaybackDeviceId(int deviceId);
    public static native void delete();
}
