package cn.edu.whu.lmars.acousticloc;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.support.v4.app.ActivityCompat;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ToggleButton;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import cn.edu.whu.lmars.acousticlocsdk.AcousticsEngine;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();
    private static final int AUDIO_EFFECT_REQUEST = 0;

    private boolean isRecording = false;

    private ToggleButton toggleAudioRecorderButton;
    private View savingRecordView;
    private EditText savingRecordEditText;

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


        if (isRecording) {

        } else {

        }

        AlertDialog savingRecordDialog = onCreateSavingRecordDialog();
//        savingRecordDialog.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
//        savingRecordDialog.setOnShowListener(new DialogInterface.OnShowListener() {
//            public void onShow(DialogInterface dialog) {
//                if (savingRecordEditText != null) {
//                    savingRecordEditText.setFocusable(true);
//                    savingRecordEditText.setFocusableInTouchMode(true);
//                    savingRecordEditText.requestFocus();
//
//
//                    InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
//                    imm.showSoftInput(savingRecordEditText, InputMethodManager.SHOW_IMPLICIT);
//                }
//            }
//        });

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
        String mSavingRecordFileName = getCustomTimeStamp();
        savingRecordEditText = savingRecordView.findViewById(R.id.edit_text_saving_record);
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

            }
        });
        builder.setNeutralButton(R.string.save_record_audio_dialog_neutral_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User cancelled the dialog
                dialog.cancel();
            }
        });
        builder.setNegativeButton(R.string.save_record_audio_dialog_negative_button_text, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // User clicked DELETE button
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
