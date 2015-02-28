package org.asmlibrary.fit;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.InputStream;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
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
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.objdetect.CascadeClassifier;
import org.asmlibrary.fit.R;
import org.asmlibrary.fit.ASMFit;

import android.os.Handler;
import android.os.Message;   
import android.hardware.Camera;
import android.content.Intent;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout.LayoutParams;
import android.net.Uri;  
import android.database.Cursor;    
import android.graphics.Bitmap;    
import android.graphics.BitmapFactory;
import android.provider.MediaStore;
import android.widget.Toast;

public class ASMLibraryActivity extends Activity implements CvCameraViewListener2
{    
	private static final String    TAG                 = "ASMLibraryDemo";
    
    private Mat                    	mRgba;
    private Mat                    	mSmallRgba;
    private Mat                    	mGray;
    private File                   	mCascadeFile;
    private File                   	mFastCascadeFile;
    private File                   	mModelFile;
    private File                   	mAAMModelFile;
    private int						m_NumberOfCameras = 0;
    private long				   	mFrame;
    private boolean					mFlag;
    private boolean					mFastDetect = false;
    private boolean					mCamera = true;
    private boolean					mAvatar = false;
    private int						mCameraIdx = 0;
    private Mat						mShape;
    private static final Scalar 	mRedColor = new Scalar(255, 0, 0);
    private static final Scalar 	mCyanColor = new Scalar(0, 255, 255);
    private MenuItem               	mHelpItem;
    private MenuItem               	mDetectItem;
    private MenuItem				mCameraitem;
    private MenuItem				mAlbumItem;
    private MenuItem				mChooseItem;
    private MenuItem				mAvatarItem;
    private JavaCameraView   		mOpenCvCameraView;
    private JavaCameraView   		mOpenCvFrontCameraView;
    private MatrixImageView   		mImageView;
    private LoadingCircleView 		mLoadingView;
    private CascadeClassifier 		mJavaCascade;
    
    public ASMLibraryActivity() 
    {
        Log.i(TAG, "Instantiated new " + this.getClass());
    }
    
    private File getSourceFile(int id, String name, String folder)
    {
    	File cascadeDir = getDir(folder, Context.MODE_PRIVATE);
        File file = new File(cascadeDir, name);
        boolean existed = true;
        try {
			FileInputStream fis=new FileInputStream(file);
		} catch (FileNotFoundException e) {
			//e.printStackTrace();
			existed = false;
		}
        if(existed == true)
        	return file;
        
		try 
		{
    		InputStream is = getResources().openRawResource(id);
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
       mModelFile = getSourceFile(R.raw.my68_1d, "my68_1d.amf", "model");
        if(mModelFile != null)
        	ASMFit.nativeReadModel(mModelFile.getAbsolutePath());   

        mAAMModelFile = getSourceFile(R.raw.my68, "my68.aam", "model");
        if(mAAMModelFile != null)
        	ASMFit.nativeReadAAMModel(mAAMModelFile.getAbsolutePath());   
        
        mCascadeFile = getSourceFile(R.raw.haarcascade_frontalface_alt2, 
        		"haarcascade_frontalface_alt2.xml", "model");
        if(mCascadeFile != null)
        {
        	ASMFit.nativeInitCascadeDetector(mCascadeFile.getAbsolutePath());
        	mJavaCascade = new CascadeClassifier(mCascadeFile.getAbsolutePath());
        	if (mJavaCascade.empty())
                mJavaCascade = null;
        }

        mFastCascadeFile = getSourceFile(R.raw.lbpcascade_frontalface, 
        		"lbpcascade_frontalface.xml", "model");
        if(mFastCascadeFile != null)
        	ASMFit.nativeInitFastCascadeDetector(mFastCascadeFile.getAbsolutePath());
    }
    
    private static final int MSG_SUCCESS = 0;  
    private static final int MSG_FAILURE = 1;
    private static final int MSG_PROGRESS = 2;
    private static final int MSG_STATUS = 3;
    
    private Handler mHandler = new Handler() {  
    	public void handleMessage (Message msg) {
    		switch(msg.what) {
    			case MSG_SUCCESS:
    				mFittingDone = true;
    				mLoadingView.setVisibility(SurfaceView.GONE);
    				mImageView.setImageBitmap((Bitmap) msg.obj);  
    				Toast.makeText(getApplication(), "Fitting Done", Toast.LENGTH_LONG).show();  
    				break; 
    			case MSG_FAILURE:  
    				mFittingDone = true;
    				mLoadingView.setVisibility(SurfaceView.GONE);
    				Toast.makeText(getApplication(), "Canot detect any face", Toast.LENGTH_LONG).show();  
    				break;  
    			case MSG_PROGRESS:
    				mLoadingView.setProgress(msg.arg1);
    				break;
		    	case MSG_STATUS:
					mLoadingView.setStatus(msg.arg1 == 0 ? "Detecting faces" : "Face alignment");
					break;
    		}
		}  
    };  

    private String mImageFileName = new String();
    private boolean mFittingDone = false;
    private Bitmap	mBitmap = null;
    private int mScaleFactor = 1;
    
    private void fittingOnStaticImageAsyn(String imgName){
    	mImageFileName = imgName;
    	mFittingDone = false;
       	if(mBitmap != null)
    		mBitmap.recycle();
    	System.gc();
    	
    	int []factors = {1, 2, 4, 8, 16};
    	BitmapFactory.Options opt = new BitmapFactory.Options();
    	for(int i = 0; i < factors.length; i++)
    	{
    		boolean ok = true;
    		try
    		{
    			if(i == 0)
    				mBitmap = BitmapFactory.decodeFile(imgName);
    			else
    			{
    				opt.inSampleSize = factors[i];
    				mBitmap = BitmapFactory.decodeFile(imgName, opt);
    			}
    		}
    		catch(OutOfMemoryError e)
    		{
    			ok = false;
    		}
    		
    		if(ok == true)
    		{
    			mScaleFactor = factors[i];
    			break;
    		}
    	}

        mImageView.setImageBitmap(mBitmap);
    	mLoadingView.setVisibility(SurfaceView.VISIBLE);
    	
    	new Thread(new Runnable() {  
    		@Override  
    		public void run() {
    			int i = 0;
    			while (!mFittingDone) {
					try {
						i += 4;
						int j = i % 200;
						if(j >= 100) j = 200 - j;
						mHandler.obtainMessage(MSG_PROGRESS, j, 0).sendToTarget(); 
						Thread.sleep(200);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
    		}  
    	}).start();  
    	
    	new Thread(new Runnable() {  
    		@Override  
    		public void run() {
    			Mat image = Highgui.imread(mImageFileName, Highgui.IMREAD_COLOR);
    	    	Mat shapes = new Mat();
    	    	mHandler.obtainMessage(MSG_STATUS, 0, 0).sendToTarget(); 
    	    	boolean flag = ASMFit.detectAll(image, shapes);
    	    	
    	    	if(flag == false){
    	    		mHandler.obtainMessage(MSG_FAILURE).sendToTarget();  
    	    		return;
    	    	}
    	    	
    	    	mHandler.obtainMessage(MSG_STATUS, 1, 0).sendToTarget(); 
    	    	ASMFit.fitting(image, shapes, 30);
            	for(int i = 0; i < shapes.rows(); i++){
            		for(int j = 0; j < shapes.row(i).cols()/2; j++){
            			double x = shapes.get(i, 2*j)[0];
        				double y = shapes.get(i, 2*j+1)[0];
        				Point pt = new Point(x, y);
        				
        				Core.circle(image, pt, 3, mCyanColor, 2);
            		}
            	}
            	
            	Imgproc.cvtColor(image, image, Imgproc.COLOR_RGB2BGR);
            	if(mScaleFactor == 1)
            		Utils.matToBitmap(image, mBitmap);
            	else
            	{
            		Mat image2 = new Mat(mBitmap.getHeight(), mBitmap.getWidth(), image.type());
            		Imgproc.resize(image, image2, image2.size());
            		Utils.matToBitmap(image2, mBitmap);
            		image2.release();
            	}
            	
            	image.release();
            	shapes.release();
    	    	mHandler.obtainMessage(MSG_SUCCESS, mBitmap).sendToTarget();
    		}  
    	}).start();  
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
        mOpenCvCameraView.setVisibility(SurfaceView.GONE);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        if (m_NumberOfCameras > 1) 
        {
        	mOpenCvFrontCameraView = (JavaCameraView) findViewById(R.id.java_surface_front_view);            
        	mOpenCvFrontCameraView.setVisibility(SurfaceView.GONE);
        	mOpenCvFrontCameraView.setCvCameraViewListener(this);
        	mOpenCvFrontCameraView.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        }
        
        mImageView = (MatrixImageView)findViewById(R.id.image_view);
        mImageView.setVisibility(SurfaceView.GONE);
        mImageView.setLayoutParams(new android.widget.FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        
        mLoadingView = (LoadingCircleView) findViewById(R.id.loading_cirle_view);
        mLoadingView.setVisibility(SurfaceView.GONE);
        
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
        //if(OpenCVLoader.initDebug())
        //	initialize();
        
        mFrame = 0;
        mFlag = false;
        
        if(mCamera)
        {
            mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
	    }
        else
        {
             mImageView.setVisibility(SurfaceView.VISIBLE);
        }
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
        
        if(mCamera)
        {
	        mOpenCvCameraView.enableView();
	        
	        if(m_NumberOfCameras > 1)    
	            mOpenCvFrontCameraView.enableView();
	
	        mFrame = 0;
	        mFlag = false;
        }
        else
        {
        	 mImageView = (MatrixImageView)findViewById(R.id.image_view);
             mImageView.setVisibility(SurfaceView.VISIBLE);
        }
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
    	mChooseItem = menu.add(mCamera ? "Image Fitting" : "Video Fitting");
    	mAvatarItem = menu.add(mAvatar ? "Track" : "Avatar");
        mCameraitem = menu.add(mCameraIdx == 0 ? "Front Camera" : "Back Camera");
        mAlbumItem = menu.add("Pick Album");
        mDetectItem = menu.add(mFastDetect ? "JavaCascadeDetector" : "FastCascadeDetector");
        mHelpItem = menu.add("About ASMLibrary");
        
        if(mCamera)
        {
        	mCameraitem.setVisible(m_NumberOfCameras > 1);
        	mAlbumItem.setVisible(false);
        	mAvatarItem.setVisible(true);
        }
        else
        {
        	mAlbumItem.setVisible(true);
        	mCameraitem.setVisible(false);
        	mAvatarItem.setVisible(false);
        }
        
        return true;
    }
    
    
    private static final int SELECT_PICTURE = 1;
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	super.onActivityResult(requestCode, resultCode, data); 
    	if(resultCode == RESULT_OK && requestCode == SELECT_PICTURE) {           
	        Uri uri = data.getData();	                
	        if( uri == null ) {
	            return;
	        }
	        
	        // try to retrieve the image from the media store first
	        // this will only work for images selected from gallery
	        String[] projection = { MediaStore.Images.Media.DATA };
	        Cursor cursor = getContentResolver().query(uri, projection, null, null, null);

	        String path = null;
	        if( cursor != null ){
	            int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
	            cursor.moveToFirst();
	            path = cursor.getString(column_index);
            	cursor.close();
	        }
	        if(path == null)
	        	path = uri.getPath();
	        
	        fittingOnStaticImageAsyn(path);
	    }
    }
    
    private void chooseImageFromAlbum()
    {
    	Intent intent = new Intent(Intent.ACTION_PICK);
    	intent.setType("image/*");
    	startActivityForResult(Intent.createChooser(intent, "Select Picture"), SELECT_PICTURE);  
    }

    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) 
    {
    	if(item == mChooseItem)
    	{
    		mCamera = !mCamera;
    		mChooseItem.setTitle(mCamera ? "Image Fitting" : "Video Fitting");
    		if(mCamera)
            {
    			mCameraitem.setVisible(m_NumberOfCameras > 1);
            	mAlbumItem.setVisible(false);
            	mAvatarItem.setVisible(true);
            	
            	mImageView.setVisibility(SurfaceView.GONE);
            	mCameraIdx = 0;
               	mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
                mOpenCvCameraView.enableView();
                if (m_NumberOfCameras > 1)
                {
                	mOpenCvFrontCameraView.setVisibility(SurfaceView.GONE);
                	mOpenCvFrontCameraView.enableView();
                }
             }
            else
            {
            	mAlbumItem.setVisible(true);
            	mCameraitem.setVisible(false);
            	mAvatarItem.setVisible(false);
            	
            	if (m_NumberOfCameras > 1)
                    mOpenCvFrontCameraView.setVisibility(SurfaceView.GONE);
            	mOpenCvCameraView.setVisibility(SurfaceView.GONE);
            	mImageView.setVisibility(SurfaceView.VISIBLE);
            	
            	chooseImageFromAlbum();  
            }
    	}
    	else if(item == mAlbumItem)
    	{
    		chooseImageFromAlbum();  
    	}
    	else if (item == mHelpItem)
        	new AlertDialog.Builder(this).setTitle("About ASMLibrary")
        		.setMessage("ASMLibrary -- A compact SDK for face alignment/tracking\n" +
        				"Copyright (c) 2008-2015 by Yao Wei, all rights reserved.\n" +
        				"Contact: njustyw@gmail.com\n")
        				.setPositiveButton("OK", null).show();
        else if(item == mDetectItem)
        {
        	mFastDetect = !mFastDetect;
        	mDetectItem.setTitle(mFastDetect ? "JavaCascadeDetector" : "FastCascadeDetector");
        }
        else if(item == mAvatarItem)
        {
        	mAvatar = !mAvatar;
        	mAvatarItem.setTitle(mAvatar ? "Track" : "Avatar");
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
        mSmallRgba = new Mat();
        mShape = new Mat();
        mFrame = 0;
        mFlag = false;
    }

    public void onCameraViewStopped() 
    {
        mGray.release();
        mRgba.release();
        mSmallRgba.release();
        mShape.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) 
    {
        mRgba = inputFrame.rgba();
        if(mCameraIdx == 1)
			Core.flip(mRgba, mRgba, 1);
        Imgproc.cvtColor(mRgba, mGray, Imgproc.COLOR_RGBA2GRAY);
        Mat submat = null;
        if(mAvatar)
    	{
        	int w = mRgba.cols()/4;
        	int h = mRgba.rows()/4;
        	if(mSmallRgba.rows() != h)
        		mSmallRgba.create(w, h, mRgba.type());
        	Imgproc.resize(mRgba, mSmallRgba, new Size(w, h));
        	submat = mRgba.submat(new Rect(3*w, 0, w, h));
    	}
        
        long lMilliStart = System.currentTimeMillis();
        
        if(mFrame == 0 || mFlag == false)
		{
        	Mat detShape = new Mat();
        	int iFaceIndex = 0;
        	if(mFastDetect || mJavaCascade == null)
				mFlag = ASMFit.fastDetectAll(mGray, detShape);
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
                
                	ASMFit.initShape(detShape);
                }
			}
			if(mFlag)	mShape = detShape.row(iFaceIndex);
		}
			
		if(mFlag) 
		{
			mFlag = ASMFit.videoFitting(mGray, mShape, mFrame, 20);
		}
		
		if(mFlag)
		{
			if(mAvatar)
			{
				ASMFit.drawAvatar(mRgba, mShape);
				mSmallRgba.copyTo(submat);
			}
			else
			{
				int nPoints = mShape.row(0).cols()/2;
				for(int i = 0; i < nPoints; i++)
				{ 
					double x = mShape.get(0, 2*i)[0];
					double y = mShape.get(0, 2*i+1)[0];
					Point pt = new Point(x, y);
					
					Core.circle(mRgba, pt, 3, mCyanColor, 2);
				}
			}
		}
		else
		{
			if(mAvatar)
			{
				mRgba.setTo(new Scalar(0, 0, 0));
				mSmallRgba.copyTo(submat);
			}
		}
		
		long lMilliNow = System.currentTimeMillis();	
    	String string = String.format("FPS: %2.1f", 1000.0f / (float)(lMilliNow - lMilliStart));
        double dTextScaleFactor = 1.8;
	    Core.putText(mRgba, string, new Point(10, dTextScaleFactor*60*1), 
   			 Core.FONT_HERSHEY_SIMPLEX, dTextScaleFactor, mCyanColor, 2);
		
		mFrame ++;
		return mRgba;
    }
}