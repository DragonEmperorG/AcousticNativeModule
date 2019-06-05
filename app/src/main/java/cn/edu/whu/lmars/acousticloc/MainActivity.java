package cn.edu.whu.lmars.acousticloc;

import android.Manifest;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();
    private static final int AUDIO_EFFECT_REQUEST = 0;

    private ToggleButton toggleAudioRecorderButton;
    private boolean isRecording = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        toggleAudioRecorderButton = findViewById(R.id.button_toggle_audio_recorder);
        toggleAudioRecorderButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Log.d(TAG, "Current toggleAudioRecorderButton Status: " + isChecked );
                if(isChecked){
                    //
                    toggleAudioRecorderButton.setChecked(true);
                    Log.d(TAG, "StartRecordAudio toggleAudioRecorderButton Status: true");
                    startRecordAudio();
                }
                else{
                    //
                    toggleAudioRecorderButton.setChecked(false);
                    Log.d(TAG, "StopRecordAudio toggleAudioRecorderButton Status: false");
                    stopRecordAudio();
                }
            }
        });

        AcousticsEngine.create();
    }

    private void startRecordAudio() {
        Log.d(TAG, "Attempting to start");

        if (!isRecordPermissionGranted()){
            requestRecordPermission();
            return;
        }

        isRecording = true;
        AcousticsEngine.setRecordAudioOn(true);

    }

    private void stopRecordAudio() {
        Log.d(TAG, "Playing, attempting to stop");

        isRecording = false;
        AcousticsEngine.setRecordAudioOn(false);

    }

    private boolean isRecordPermissionGranted() {
        return (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) ==
                PackageManager.PERMISSION_GRANTED);
    }

    private void requestRecordPermission(){
        ActivityCompat.requestPermissions(
                this,
                new String[]{Manifest.permission.RECORD_AUDIO},
                AUDIO_EFFECT_REQUEST);
    }
}
