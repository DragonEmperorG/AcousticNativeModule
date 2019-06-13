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
    public static native void initialRecordAudio();
    public static native void startRecordAudio();
    public static native void pauseRecordAudio();
    public static native void stopRecordAudio();
    public static native void saveRecordAudio(String recordAudioFilePath);
    public static native void setRecordingDeviceId(int deviceId);
    public static native void setPlaybackDeviceId(int deviceId);
    public static native short[] readPaintRecordAudioWaveBuffer(int offsetInShorts, int sizeInShorts);
    public static native void delete();
}
