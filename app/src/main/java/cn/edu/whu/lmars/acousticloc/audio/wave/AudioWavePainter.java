package cn.edu.whu.lmars.acousticloc.audio.wave;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.AsyncTask;
import android.os.Handler.Callback;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.util.ArrayList;
import java.util.Date;

import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

public class AudioWavePainter {
    private static final String TAG = AudioWavePainter.class.getName();

    // Painting Child Thread Tag
    public boolean mIsPainting;
    // Time Coordinate
    private long mCurrentTimeStamp;
    // Painting Interval(ms)
    private int mPaintInterval;
    // Pixel / Sample
    private float mPixelPerSample;
    // Painting Down Sampling Rate (Interval between two painted samples)
    private int mPaintDownSamplingRate;
    // Painting Buffer Size Read from SoundRecord
    private int mPaintBufferSize;

    // Painting Buffer Data
    private ArrayList<Float> mAudioBuffer = new ArrayList<>();
    // Editor Panel Painter Config (播放指示器)
    private Paint mRecordingIndicatorPainter;
    // Draw Waveform Foreground (波形前景)
    private Paint mWaveformForegroundPainter;

    public AudioWavePainter() {

        mIsPainting = false;

        mPaintInterval = 0;
        mPaintDownSamplingRate = 1;
        mPaintBufferSize = 0;

        mRecordingIndicatorPainter = new Paint();
        mRecordingIndicatorPainter.setColor(0xBF0000);
        mWaveformForegroundPainter = new Paint();
        mWaveformForegroundPainter.setColor(0xFF4BF3A7);
    }

    public void startPaintAudioWave(int _recordBufSize, SurfaceView _surfaceView) {
        mIsPainting = true;
        new AudioWavePaintingTask(_recordBufSize, _surfaceView, mWaveformForegroundPainter).execute();
    }

    public void pausePaintAudioWave() {
        mIsPainting = false;
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
                float[] tempBuffer = new float[tRecordingBufSize];
                while (mIsPainting) {
                    mPaintBufferSize = AcousticsEngine.readPaintRecordAudioWaveBuffer(tempBuffer, 0, tRecordingBufSize);
                    Log.i(TAG, "doInBackground: paintBufferSize = " + mPaintBufferSize);
                    synchronized (mAudioBuffer) {
                        for (int i = 0; i < mPaintBufferSize; i += mPaintDownSamplingRate) {
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
                ArrayList<Float> updateBuffer = new ArrayList<>();
                Log.i(TAG, "onProgressUpdate: " + mAudioBuffer.size());
                synchronized (mAudioBuffer) {
                    if (mAudioBuffer.size() == 0)
                        return;
                    while(mAudioBuffer.size() > tSurfaceView.getWidth() / mPixelPerSample){
                        mAudioBuffer.remove(0);
                    }
                    for (Float mAudioSample : mAudioBuffer) {
                        updateBuffer.add(Float.valueOf(mAudioSample.toString()));
                    }
                }
                updateAudioWaveCanvas(updateBuffer);
                mCurrentTimeStamp = new Date().getTime();
            }
            super.onProgressUpdate(values);
        }

        private void updateAudioWaveCanvas(ArrayList<Float> waveBuffer) {
            if (!mIsPainting)
                return;

            SurfaceHolder tSurfaceHolder = tSurfaceView.getHolder();
            int pCanvasWidth = tSurfaceView.getWidth();
            int pCanvasHeight = tSurfaceView.getHeight();
            float pYCenterLine = pCanvasHeight * 0.5f;
            mPixelPerSample = pCanvasWidth / (44100 * 4 * 2.0f) * mPaintDownSamplingRate;
            Canvas tCanvas = tSurfaceHolder.lockCanvas();
            if (tCanvas == null)
                return;

            int pStart = (int) (waveBuffer.size() * mPixelPerSample);
            if(pCanvasWidth - pStart <= 20) {
                pStart = pCanvasWidth - 20;
            }

            tCanvas.drawLine(pStart, 0, pStart, pCanvasHeight, mRecordingIndicatorPainter);

            for (int i = 0; i < waveBuffer.size(); i++) {
                float pX = i * mPixelPerSample;
                if(pCanvasWidth - (i-1) * mPixelPerSample <= 0){
                    pX = pCanvasWidth;
                }

                float pY = waveBuffer.get(i) * pCanvasHeight * 0.5f;

                tCanvas.drawLine(pX, pYCenterLine - pY, pX, pYCenterLine + pY, tPaint);

            }

            tSurfaceHolder.unlockCanvasAndPost(tCanvas);
        }

    }
}
