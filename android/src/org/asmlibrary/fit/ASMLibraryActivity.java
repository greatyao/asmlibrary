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
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.core.Point;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.objdetect.CascadeClassifier;
import org.asmlibrary.fit.R;
import org.asmlibrary.fit.ASMFit;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;

public class ASMLibraryActivity extends Activity implements CvCameraViewListener2{
    
	private static final String    TAG                 = "ASMLibraryDemo";
    
    private Mat                    mRgba;
    private Mat                    mGray;
    private File                   mCascadeFile;
    private File                   mModelFile;
    private ASMFit      		   mASMFit;
    private long				   mFrame;
    private boolean					mFlag;
    private Mat						mShape;
    private static final Scalar mColor = new Scalar(255, 0, 0);
    
    
    private CameraBridgeViewBase   mOpenCvCameraView;
    
    public ASMLibraryActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }
    
    private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
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

                    try {
                        // load model file from application resources
                        InputStream is = getResources().openRawResource(R.raw.my68_1d);
                        File cascadeDir = getDir("model", Context.MODE_PRIVATE);
                        mModelFile = new File(cascadeDir, "my68_1d.amf");
                        FileOutputStream os = new FileOutputStream(mModelFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();

                        mASMFit.nativeReadModel(mModelFile.getAbsolutePath());

                        cascadeDir.delete();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e(TAG, "Failed to load model. Exception thrown: " + e);
                    }

                    try {
                        // load cascade file from application resources
                        InputStream is = getResources().openRawResource(R.raw.haarcascade_frontalface_alt2);
                        File cascadeDir = getDir("cascade", Context.MODE_PRIVATE);
                        mCascadeFile = new File(cascadeDir, "haarcascade_frontalface_alt2.xml");
                        FileOutputStream os = new FileOutputStream(mCascadeFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();

                        mASMFit.nativeInitCascadeDetector(mCascadeFile.getAbsolutePath());

                        cascadeDir.delete();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e(TAG, "Failed to load cascade. Exception thrown: " + e);
                    }
                    
                    //test image alignment
                    try {
                        // load image file from application resources
                        InputStream is = getResources().openRawResource(R.raw.gump);
                        File cascadeDir = getDir("image", Context.MODE_PRIVATE);
                        File JPGFile = new File(cascadeDir, "gump.jpg");
                        FileOutputStream os = new FileOutputStream(JPGFile);

                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = is.read(buffer)) != -1) {
                            os.write(buffer, 0, bytesRead);
                        }
                        is.close();
                        os.close();

                        Mat image = Highgui.imread(JPGFile.getAbsolutePath(), Highgui.IMREAD_UNCHANGED);
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
            				
                        cascadeDir.delete();

                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.e(TAG, "Failed to load cascade. Exception thrown: " + e);
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
			mFlag = mASMFit.detectOne(mGray, mShape);
			
			if(mFlag) 
			{
				mFlag = mASMFit.videoFitting(mGray, mShape, mFrame);
				
				for(int i = 0; i < mShape.row(0).cols()/2; i++)
				{ 
					Point pt = new Point(mShape.get(0, 2*i)[0], mShape.get(0, 2*i+1)[0]);
					Core.circle(mRgba, pt, 3, mColor);
				}
			}
		}
		
		mFrame ++;

        return mRgba;
    }
}