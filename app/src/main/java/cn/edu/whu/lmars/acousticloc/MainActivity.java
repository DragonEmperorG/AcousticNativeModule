package cn.edu.whu.lmars.acousticloc;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.media.MediaRecorder;
import android.support.v4.app.ActivityCompat;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ToggleButton;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import cn.edu.whu.lmars.acousticloc.audio.wave.AudioWavePainter;
import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();
    private static final int AUDIO_EFFECT_REQUEST = 0;

    private boolean isNewRecord = true;
    private boolean isRecording = false;
    private boolean recordingStatusPrevComplete = false;

    private ToggleButton toggleAudioRecorderButton;
    private View savingRecordView;
    private EditText savingRecordEditText;

    private AudioWavePainter audioWavePainter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        toggleAudioRecorderButton = findViewById(R.id.button_toggle_audio_recorder);
        toggleAudioRecorderButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    //
                    toggleAudioRecorderButton.setChecked(true);

                    if (isNewRecord) {
                        AcousticsEngine.initialRecordAudio();
                        isNewRecord = false;
                    } else {
                        startRecordAudio();
                    }

                    recordingStatusPrevComplete = true;
                }
                else{
                    //
                    toggleAudioRecorderButton.setChecked(false);
                    Log.d(TAG, "StopRecordAudio toggleAudioRecorderButton Status: false");
                    pauseRecordAudio();
                    recordingStatusPrevComplete = false;
                }
            }
        });

        if (!isRecordPermissionGranted()){
            requestRecordPermission();
            return;
        }



        AcousticsEngine.create();
    }

    public void completeRecordAudio(View view) {
        Log.d(TAG, "Complete recording audio");

        if (isRecording) {
            pauseRecordAudio();
        }

        onCreateSavingRecordDialog();

    }

    private AlertDialog onCreateSavingRecordDialog() {

        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        // Chain together various setter methods to set the dialog characteristics
        builder.setTitle(R.string.save_record_audio_dialog_title);

        // Get the layout inflater
        LayoutInflater inflater = this.getLayoutInflater();

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        savingRecordView = inflater.inflate(R.layout.dialog_saving_record, null);

        /*
         * Set Customized EditText
         */
        String mSavingRecordFileName = getCustomTimeStamp();

        // Get the EditText by Inflated View
        savingRecordEditText = savingRecordView.findViewById(R.id.edit_text_saving_record);

        // Set default record file name
        savingRecordEditText.setText(mSavingRecordFileName.toCharArray(), 0, mSavingRecordFileName.length());
        savingRecordEditText.setSelectAllOnFocus(true);
        savingRecordEditText.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                savingRecordEditText.post(new Runnable() {
                    @Override
                    public void run() {
                        InputMethodManager inputMethodManager= (InputMethodManager) MainActivity.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                        inputMethodManager.showSoftInput(savingRecordEditText, InputMethodManager.SHOW_IMPLICIT);
                    }
                });
            }
        });
        savingRecordEditText.requestFocus();
        builder.setView(savingRecordView);

        // Add the buttons
        builder.setPositiveButton(R.string.save_record_audio_dialog_positive_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User clicked SAVE button
                AcousticsEngine.saveRecordAudio(getAudioRecordingFilePath(MainActivity.this, savingRecordEditText.getText().toString()));
                AcousticsEngine.stopRecordAudio();
                isNewRecord = true;
            }
        });
        builder.setNeutralButton(R.string.save_record_audio_dialog_neutral_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {

                if (recordingStatusPrevComplete) {
                    startRecordAudio();
                }
                // User cancelled the dialog
                dialog.cancel();
            }
        });
        builder.setNegativeButton(R.string.save_record_audio_dialog_negative_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User clicked DELETE button
                AcousticsEngine.stopRecordAudio();


                isNewRecord = true;
            }
        });

        builder.show();

        // Return the AlertDialog from create()
        return builder.create();
    }

    private String getCustomTimeStamp() {
        SimpleDateFormat customDateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.CHINA);
        Date mDate = new Date();
        return customDateFormat.format(mDate);
    }

    private String getAudioRecordingFilePath(Context context, String recordFileName) {

        File recordFileDirectory = new File(context.getExternalFilesDir(null), "AudioLMARS");
        if (!recordFileDirectory.exists()) {
            boolean directoryCreationStatus = recordFileDirectory.mkdirs();
            Log.i(TAG, "getAudioRecordingFilePath: directoryCreationStatus: " + directoryCreationStatus);
        }

        File recordFilePath = new File(recordFileDirectory, recordFileName + ".wav");

        if (recordFilePath.exists()) {
            boolean deletionStatus = recordFilePath.delete();
            Log.i(TAG, "getAudioRecordingFilePath: File already exists, delete it first, deletionStatus: " + deletionStatus);
        }

        if (!recordFilePath.exists()) {
            try {
                boolean deletionStatus = recordFilePath.createNewFile();
                Log.i(TAG, "getAudioRecordingFilePath: fileCreationStatus: " + deletionStatus);
            } catch (Exception e) {
                Log.i(TAG, "getAudioRecordingFilePath: createNewFile failed: " + e.toString());
            }
        }

        return recordFilePath.getAbsolutePath();
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

        AcousticsEngine.startRecordAudio();
        isRecording = true;
    }

    private void pauseRecordAudio() {
        Log.d(TAG, "Attempting to pause");

        AcousticsEngine.pauseRecordAudio();
        isRecording = false;
    }

    private void stopRecordAudio() {
        Log.d(TAG, "Playing, attempting to stop");




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
