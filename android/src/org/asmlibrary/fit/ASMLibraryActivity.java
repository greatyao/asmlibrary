package org.asmlibrary.fit;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.core.Scalar;
import org.opencv.core.Point;
import org.opencv.core.CvType;
import org.opencv.imgproc.Imgproc;
import android.hardware.Camera;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.objdetect.CascadeClassifier;
import org.asmlibrary.fit.R;
import org.asmlibrary.fit.ASMFit;

import android.app.Activity;
import android.util.DisplayMetrics;
import android.content.res.Configuration;  
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.LinearLayout.LayoutParams;

public class ASMLibraryActivity extends Activity implements CvCameraViewListener2
{    
	private static final String    TAG                 = "ASMLibraryDemo";
    
    private Mat                    	mRgba;
    private Mat                    	mGray;
    private File                   	mCascadeFile;
    private File                   	mFastCascadeFile;
    private File                   	mModelFile;
    private ASMFit      		   	mASMFit;
    private int						m_NumberOfCameras = 0;
    private long				   	mFrame;
    private boolean					mFlag;
    private boolean					mFastDetect = false;
    private int						mCameraIdx = 0;
    private Mat						mShape;
    private static final Scalar 	mRedColor = new Scalar(255, 0, 0);
    private static final Scalar 	mGreenColor = new Scalar(0, 255, 0);
    private MenuItem               	mHelpItem;
    private MenuItem               	mDetectItem;
    private MenuItem				mCameraitem;
    private JavaCameraView   		mOpenCvCameraView;
    private JavaCameraView   		mOpenCvFrontCameraView;
    private CascadeClassifier 		mJavaCascade;
    
    public ASMLibraryActivity() 
    {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }
    
    private File getSourceFile(int id, String name, String folder)
    {
		File file = null;
		try 
		{
    		InputStream is = getResources().openRawResource(id);
            File cascadeDir = getDir(folder, Context.MODE_PRIVATE);
            file = new File(cascadeDir, name);
            FileOutputStream os = new FileOutputStream(file);
            
            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) 
            {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();
		}catch (IOException e) 
		{
            e.printStackTrace();
            Log.e(TAG, "Failed to load file " + name + ". Exception thrown: " + e);
        }
            
        return file;	
	}
    
    private void initialize()
    {
    	mASMFit = new ASMFit();

        mModelFile = getSourceFile(R.raw.my68_1d, "my68_1d.amf", "model");
        if(mModelFile != null)
        	mASMFit.nativeReadModel(mModelFile.getAbsolutePath());   
        
        mCascadeFile = getSourceFile(R.raw.haarcascade_frontalface_alt2, 
        		"haarcascade_frontalface_alt2.xml", "cascade");
        if(mCascadeFile != null)
        {
        	mASMFit.nativeInitCascadeDetector(mCascadeFile.getAbsolutePath());
        	mJavaCascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
        	if (mJavaCascade.empty())
                mJavaCascade = null;
        }

        mFastCascadeFile = getSourceFile(R.raw.lbpcascade_frontalface, 
        		"lbpcascade_frontalface.xml", "cascade");
        if(mFastCascadeFile != null)
        	mASMFit.nativeInitFastCascadeDetector(mFastCascadeFile.getAbsolutePath());
        
        mCascadeFile.delete();
        mFastCascadeFile.delete();
        
        //test image alignment
        // load image file from application resources
    	File JPGFile = getSourceFile(R.raw.gump, "gump.jpg", "image");
    	
    	Mat image = Highgui.imread(JPGFile.getAbsolutePath(), Highgui.IMREAD_GRAYSCALE);
        Mat shapes = new Mat();
        
        if(mASMFit.detectAll(image, shapes) == true)
			mASMFit.fitting(image, shapes, 30);

        mOpenCvCameraView.enableView();
        
        if(m_NumberOfCameras > 1)    
            mOpenCvFrontCameraView.enableView();
    }
    
    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) 
    {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    initialize();
                    break;
                } 
                default:
                {
                    super.onManagerConnected(status);
                    break;
                }
            }
        }
    };

	/** Called when the activity is first created. */
	@Override
    public void onCreate(Bundle savedInstanceState) 
	{
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        m_NumberOfCameras = Camera.getNumberOfCameras();
        
        setContentView(R.layout.asmlibrary_surface_view);

        mOpenCvCameraView = (JavaCameraView) findViewById(R.id.java_surface_back_view);
        
        mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        
        mOpenCvCameraView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        if (m_NumberOfCameras > 1) 
        {
        	mOpenCvFrontCameraView = (JavaCameraView) findViewById(R.id.java_surface_front_view);
            
        	mOpenCvFrontCameraView.setVisibility(SurfaceView.GONE);
        	mOpenCvFrontCameraView.setCvCameraViewListener(this);
        	mOpenCvFrontCameraView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        }
        
        mFrame = 0;
        mFlag = false;
    }
	
	@Override
    public void onPause()
    {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        
        if (m_NumberOfCameras > 1 && mOpenCvFrontCameraView != null) 
        	mOpenCvFrontCameraView.disableView();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
        //if(OpenCVLoader.initDebug())
        //	initialize();

        mFrame = 0;
        mFlag = false;
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        mCameraitem = menu.add(mCameraIdx == 0 ? "Front Camera" : "Back Camera");
        mDetectItem = menu.add(mFastDetect ? "JavaCascadeDetector" : "FastCascadeDetector");
        mHelpItem = menu.add("About ASMLibrary");
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) 
    {
        if (item == mHelpItem)
        	new AlertDialog.Builder(this).setTitle("About ASMLibrary")
        		.setMessage("ASMLibrary -- A compact SDK for face alignment/tracking\n" +
        				"Copyright (c) 2008-2011 by Yao Wei, all rights reserved.\n" +
        				"Contact: njustyw@gmail.com\n")
        				.setPositiveButton("OK", null).show();
        else if(item == mDetectItem)
        {
        	mFastDetect = !mFastDetect;
        	mDetectItem.setTitle(mFastDetect ? "JavaCascadeDetector" : "FastCascadeDetector");
        }
        else if(item == mCameraitem)
        {
        	if (m_NumberOfCameras > 1) 
        	{
    			if (mCameraIdx == 0) 
    			{
	    			mOpenCvCameraView.setVisibility(SurfaceView.GONE);
		    		mOpenCvFrontCameraView = (JavaCameraView) findViewById(R.id.java_surface_front_view);
		    		mOpenCvFrontCameraView.setCvCameraViewListener(this);
		    		mOpenCvFrontCameraView.setVisibility(SurfaceView.VISIBLE);
				
				    mCameraIdx = 1;
				    mCameraitem.setTitle("Back Camera");
	    		 }
		    	else 
		    	{
			    	mOpenCvFrontCameraView.setVisibility(SurfaceView.GONE);
				    mOpenCvCameraView = (JavaCameraView) findViewById(R.id.java_surface_back_view);
    				mOpenCvCameraView.setCvCameraViewListener(this);
	    			mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
		    		
	    			mCameraIdx = 0;
				    mCameraitem.setTitle("Front Camera");
	    		}
    		}
        }
        return true;
    }

    @Override
    public void onDestroy() 
    {
        super.onDestroy();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
        
        if (m_NumberOfCameras > 1 && mOpenCvFrontCameraView != null) 
        	mOpenCvFrontCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) 
    {
        mGray = new Mat();
        mRgba = new Mat();
        mShape = new Mat();
        mFrame = 0;
        mFlag = false;
    }

    public void onCameraViewStopped() 
    {
        mGray.release();
        mRgba.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) 
    {
        mRgba = inputFrame.rgba();
        if(mCameraIdx == 1)
			Core.flip(mRgba, mRgba, 1);
        Imgproc.cvtColor(mRgba, mGray, Imgproc.COLOR_RGBA2GRAY);
        
        long lMilliStart = System.currentTimeMillis();
        
        if(mFrame == 0 || mFlag == false)
		{
        	Mat detShape = new Mat();
        	int iFaceIndex = 0;
        	if(mFastDetect || mJavaCascade == null)
				mFlag = mASMFit.fastDetectAll(mGray, detShape);
			else
			{
				int height = mGray.rows();
                double faceSize = (double) height * 0.4; 
                Size sSize = new Size(faceSize, faceSize);
                MatOfRect faces = new MatOfRect();
                mJavaCascade.detectMultiScale(mGray, faces, 1.1, 2, 2, sSize, new Size());
                Rect[] facesArray = faces.toArray();
                if(facesArray.length == 0) mFlag = false;
                else
                {
                	mFlag = true;
                	detShape = new Mat(facesArray.length, 4, CvType.CV_64FC1);
                	int iMaxFaceHeight = facesArray[0].height;
                	for(int i = 0; i < facesArray.length; i++)
                	{
                		detShape.put(i, 0, facesArray[i].x);
                		detShape.put(i, 1, facesArray[i].y);
                		detShape.put(i, 2, facesArray[i].x + facesArray[i].width);
                		detShape.put(i, 3, facesArray[i].y + facesArray[i].height);
                		
                		Core.rectangle(mRgba, facesArray[i].tl(), facesArray[i].br(), mRedColor, 3);
                		
                		if (iMaxFaceHeight < facesArray[i].height) 
                		{
                        	iMaxFaceHeight = facesArray[i].height;
                        	iFaceIndex = i;
                        }
                	}
                
                	mASMFit.nativeInitShape(detShape.getNativeObjAddr());
                }
			}
			if(mFlag)	mShape = detShape.row(iFaceIndex);
		}
			
		if(mFlag) 
		{
			mFlag = mASMFit.videoFitting(mGray, mShape, mFrame, 25);
		}
		
		if(mFlag)
		{
			int nPoints = mShape.row(0).cols()/2;
			for(int i = 0; i < nPoints; i++)
			{ 
				double x = mShape.get(0, 2*i)[0];
				double y = mShape.get(0, 2*i+1)[0];
				Point pt = new Point(x, y);
				
				Core.circle(mRgba, pt, 3, mGreenColor);
			}
		}
		
		long lMilliNow = System.currentTimeMillis();	
    	String string = String.format("FPS: %2.1f", 1000.0f / (float)(lMilliNow - lMilliStart));
        double dTextScaleFactor = 1.8;
	    Core.putText(mRgba, string, new Point(10, dTextScaleFactor*60*1), 
   			 Core.FONT_HERSHEY_SIMPLEX, dTextScaleFactor, mGreenColor, 2);
		
		mFrame ++;
		return mRgba;
    }
}