package org.asmlibrary.fit;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.core.Scalar;
import org.opencv.core.Point;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.asmlibrary.fit.R;
import org.asmlibrary.fit.ASMFit;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;

public class ASMLibraryActivity extends Activity implements CvCameraViewListener2{
    
	private static final String    TAG                 = "ASMLibraryDemo";
    
    private Mat                    	mRgba;
    private Mat                    	mGray;
    private File                   	mCascadeFile;
    private File                   	mFastCascadeFile;
    private File                   	mModelFile;
    private ASMFit      		   	mASMFit;
    private long				   	mFrame;
    private boolean					mFlag;
    private boolean					mFastDetect = false;
    private Mat						mShape;
    private static final Scalar 	mColor = new Scalar(255, 0, 0);
    private MenuItem               	mHelpItem;
    private MenuItem               	mDetectItem;
    private CameraBridgeViewBase   	mOpenCvCameraView;
    
    public ASMLibraryActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }
    
    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
    	private File getSourceFile(int id, String name, String folder){
    		File file = null;
    		try {
	    		InputStream is = getResources().openRawResource(id);
	            File cascadeDir = getDir(folder, Context.MODE_PRIVATE);
	            file = new File(cascadeDir, name);
	            FileOutputStream os = new FileOutputStream(file);
	            
	            byte[] buffer = new byte[4096];
	            int bytesRead;
	            while ((bytesRead = is.read(buffer)) != -1) {
	                os.write(buffer, 0, bytesRead);
	            }
	            is.close();
	            os.close();
    		}catch (IOException e) {
                e.printStackTrace();
                Log.e(TAG, "Failed to load file " + name + ". Exception thrown: " + e);
            }
	            
            return file;
    		
    	}
    	
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");

                    // Load native library after(!) OpenCV initialization
                    System.loadLibrary("asmlibrary");
                    System.loadLibrary("jni-asmlibrary");
                    
                    mASMFit = new ASMFit();

                    mModelFile = getSourceFile(R.raw.my68_1d, "my68_1d.amf", "model");
                    if(mModelFile != null)
                    	mASMFit.nativeReadModel(mModelFile.getAbsolutePath());
                    
                    
                    mCascadeFile = getSourceFile(R.raw.haarcascade_frontalface_alt2, 
                    		"haarcascade_frontalface_alt2.xml", "cascade");
                    if(mCascadeFile != null)
                    	mASMFit.nativeInitCascadeDetector(mCascadeFile.getAbsolutePath());

                    mFastCascadeFile = getSourceFile(R.raw.lbpcascade_frontalface, 
                    		"lbpcascade_frontalface.xml", "cascade");
                    if(mFastCascadeFile != null)
                    	mASMFit.nativeInitFastCascadeDetector(mFastCascadeFile.getAbsolutePath());
                    
                    //test image alignment
                    // load image file from application resources
                	File JPGFile = getSourceFile(R.raw.gump, "gump.jpg", "image");
                	
                	Mat image = Highgui.imread(JPGFile.getAbsolutePath(), Highgui.IMREAD_GRAYSCALE);
                    Mat shapes = new Mat();
                    
                    if(mASMFit.detectAll(image, shapes) == true)
        			{
                    	/*
                    	for(int i = 0; i < shapes.row(0).cols()/2; i++)
        				{
                        	Log.d(TAG, "before points:" + 
                        			shapes.get(0, 2*i)[0] +"," +shapes.get(0, 2*i+1)[0]);
        				}
        				*/
                    	
        				mASMFit.fitting(image, shapes);
        				
        				/*
        				for(int i = 0; i < shapes.row(0).cols()/2; i++)
        				{
                        	Log.d(TAG, "after points:" + 
                        			shapes.get(0, 2*i)[0] +"," +shapes.get(0, 2*i+1)[0]);
        				}
        				*/
        			}

                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };



	/** Called when the activity is first created. */
	@Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.face_detect_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.fd_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mFrame = 0;
        mFlag = false;
    }
	
	@Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
        mFrame = 0;
        mFlag = false;
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
        if(mFastDetect == true)
        	mDetectItem = menu.add("CascadeDetector");
        else
        	mDetectItem = menu.add("FastCascadeDetector");
        mHelpItem = menu.add("About ASMLibrary");
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);
        if (item == mHelpItem)
        	new AlertDialog.Builder(this).setTitle("About ASMLibrary")
        		.setMessage("ASMLibrary -- A compact SDK for face alignment/tracking\n" +
        				"Copyright (c) 2008-2011 by Yao Wei, all rights reserved.\n" +
        				"Contact: njustyw@gmail.com\n")
        				.setPositiveButton("OK", null).show();
        else if(item == mDetectItem)
        {
        	if(mFastDetect == false)
        		mFastDetect = true;
        	else
        		mFastDetect = false;
        }
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mGray = new Mat();
        mRgba = new Mat();
        mShape = new Mat();
        mFrame = 0;
        mFlag = false;
    }

    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mRgba = inputFrame.rgba();
        mGray = inputFrame.gray();

        if(mFrame == 0 || mFlag == false)
		{
        	Mat detShape = new Mat();
			if(mFastDetect)
				mFlag = mASMFit.fastDetectAll(mGray, detShape);
			else
				mFlag = mASMFit.detectAll(mGray, detShape);
			if(mFlag)	mShape = detShape.row(0);
		}
			
		if(mFlag) 
		{
			mFlag = mASMFit.videoFitting(mGray, mShape, mFrame);
			
			for(int i = 0; i < mShape.row(0).cols()/2; i++)
			{ 
				Point pt = new Point(mShape.get(0, 2*i)[0], mShape.get(0, 2*i+1)[0]);
				Core.circle(mRgba, pt, 3, mColor);
			}
		}
		
		mFrame ++;

        return mRgba;
    }
}