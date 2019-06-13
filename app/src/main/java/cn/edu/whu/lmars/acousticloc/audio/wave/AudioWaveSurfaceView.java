package cn.edu.whu.lmars.acousticloc.audio.wave;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;
import android.view.SurfaceHolder;

/*
 * Referred to https://github.com/cokuscz/audioWaveCanvas.git
 */

public class AudioWaveSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = AudioWaveSurfaceView.class.getName();

    private SurfaceHolder mSurfaceHolder;
    // Graphic Canvas
    private Canvas mCanvas;
    private Paint mPaint;

    // Shape Parameter
    private int mCanvasWidth;
    private int mCanvasHeight;


    // Color Parameter
    private int mBackgroundColor;
    private int mCenterLineColor;

    public AudioWaveSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // Initial member variables
        mSurfaceHolder = getHolder();
        mSurfaceHolder.addCallback(this);

        mBackgroundColor = 0xFFEEEEEE;
        mCenterLineColor = 0xFF424242;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // TODO Auto-generated method stub

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        /*
         * Initial Audio Wave Coordinate
         * Start Child Thread
        */
        initialAudioWaveCoordinateFrame(this);

    }

    private void initialAudioWaveCoordinateFrame(final SurfaceView surfaceView) {
        new Thread() {
            public void run() {
                // Get graphics canvas
                mCanvas = mSurfaceHolder.lockCanvas();
                if (mCanvas == null) {
                    return;
                }

                mCanvasWidth = surfaceView.getWidth();
                mCanvasHeight = surfaceView.getHeight();

                // Draw Waveform Background(波形背景)
                mCanvas.drawColor(mBackgroundColor);

                mPaint = new Paint();
                mPaint.setColor(mCenterLineColor);
                Log.d(TAG, "mCanvasWidth: " + mCanvasWidth);
                Log.d(TAG, "mCanvasHeight: " + mCanvasHeight);
                mCanvas.drawLine(0, mCanvasHeight * 0.5F, mCanvasWidth, mCanvasHeight * 0.5F, mPaint);

                mSurfaceHolder.unlockCanvasAndPost(mCanvas);
            }
        }.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

}
