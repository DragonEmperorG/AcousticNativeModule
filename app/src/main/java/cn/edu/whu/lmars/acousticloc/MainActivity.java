package cn.edu.whu.lmars.acousticloc;

import android.Manifest;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.ToggleButton;

import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

import static android.app.PendingIntent.getActivity;

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

    public void completeRecordAudio(View view) {
        Log.d(TAG, "Complete recording audio");

        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        // Chain together various setter methods to set the dialog characteristics
        builder.setTitle(R.string.save_record_audio_dialog_title);

        // Add the buttons
        builder.setPositiveButton(R.string.save_record_audio_dialog_neutral_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User clicked OK button
            }
        });
        builder.setNeutralButton(R.string.save_record_audio_dialog_negative_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User cancelled the dialog
            }
        });
        builder.setNegativeButton(R.string.save_record_audio_dialog_negative_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User cancelled the dialog
            }
        });

        // Get the AlertDialog from create()
        AlertDialog savingRecordDialog = builder.create();


    }

    @Override
    protected void onResume() {

        Log.d(TAG, "onResume: ");
        super.onResume();
    }

    @Override
    protected void onDestroy() {

        Log.d(TAG, "onDestroy: ");
        super.onDestroy();
        AcousticsEngine.delete();
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
