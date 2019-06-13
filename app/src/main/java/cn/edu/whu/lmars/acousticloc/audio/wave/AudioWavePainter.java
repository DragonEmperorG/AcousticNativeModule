package cn.edu.whu.lmars.acousticloc.audio.wave;

import android.graphics.Paint;
import android.os.AsyncTask;
import android.os.Handler.Callback;
import android.util.Log;
import android.view.SurfaceView;

import java.util.ArrayList;
import java.util.Date;

import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

public class AudioWavePainter {
    private static final String TAG = AudioWavePainter.class.getName();

    // Painting Child Thread Tag
    public boolean mIsPainting = false;
    // Time Coordinate
    private long mCurrentTimeStamp;
    // Painting Interval
    private short mPaintInterval;
    // Pixel / Sample
    private float mPixelPerSample = 0.2f;

    // Painting Buffer Data
    private ArrayList<Short> mAudioBuffer = new ArrayList<>();
    // Editor Panel Painter Config
    private Paint mRecordingIndicatorPainter;

    public AudioWavePainter() {
        mRecordingIndicatorPainter = new Paint();
        mRecordingIndicatorPainter.setColor(0xFF424242);
    }

    /*
     * Asynchronously paint audio wave on canvas
     */
    class AudioWavePaintingTask extends AsyncTask<Object, Object, Object> {

        //
        private int tRecordingBufSize;
        // Paint Board
        private SurfaceView tSurfaceView;
        // Paint Brush
        private Paint tPaint;
        //
        private Callback tCallback;

        public AudioWavePaintingTask(int _recordingBufSize, SurfaceView _surfaceView, Paint _paint) {
            this.tRecordingBufSize = _recordingBufSize;
            this.tSurfaceView = _surfaceView;
            this.tPaint = _paint;
            mAudioBuffer.clear();
        }

        @Override
        protected void onPreExecute() {

        }

        @Override
        protected Object doInBackground(Object... params) {
            try {
                short[] tempBuffer = new short[tRecordingBufSize];

                while (mIsPainting) {
                    tempBuffer = AcousticsEngine.readPaintRecordAudioWaveBuffer(0, tRecordingBufSize);
                    synchronized (mAudioBuffer) {
                        for (int i = 0; i < tempBuffer.length; i++) {
                            if (tempBuffer[i] == 0)
                                break;
                            mAudioBuffer.add(tempBuffer[i]);
                        }
                    }
                    publishProgress();
                }

            } catch (Exception e) {
                Log.e(TAG, "doInBackground: " + e.toString());
            }

            return null;
        }

        @Override
        protected void onProgressUpdate(Object... values) {
            long updateTimeStamp = new Date().getTime();
            if (updateTimeStamp - mCurrentTimeStamp >= mPaintInterval) {
                ArrayList<Short> updateBuffer = new ArrayList<>();
                synchronized (mAudioBuffer) {
                    if (mAudioBuffer.size() == 0)
                        return;
//                    while(mAudioBuffer.size() > tSurfaceView.getWidth() / divider){
//                        inBuf.remove(0);
//                    }
                }
            }
        }
    }

}
