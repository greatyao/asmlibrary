package org.asmlibrary.fit;

import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;

import android.util.Log;

public class ASMFit {
	
	public ASMFit(){}
	
	public boolean detectAll(Mat imageGray, Mat faces){
		return nativeDetectAll(imageGray.getNativeObjAddr(), 
				faces.getNativeObjAddr());
	}
	
	public boolean detectOne(Mat imageGray, Mat face){
		return nativeDetectOne(imageGray.getNativeObjAddr(), 
				face.getNativeObjAddr());
	}
	
	public void fitting(Mat imageGray, Mat shapes){
		nativeFitting(imageGray.getNativeObjAddr(), 
				shapes.getNativeObjAddr());
	}
	
	public boolean videoFitting(Mat imageGray, Mat shape, long frame){
		return nativeVideoFitting(imageGray.getNativeObjAddr(),
				shape.getNativeObjAddr(), frame);
	}
	
	public static native boolean nativeReadModel(String modelName);
	
	public static native boolean nativeInitCascadeDetector(String cascadeName);
	public static native void nativeDestroyCascadeDetector();
	
	private static native boolean nativeDetectAll(long inputImage, long faces);
	private static native boolean nativeDetectOne(long inputImage, long face);
	
	private static native void nativeFitting(long inputImage, long shapes);
	private static native boolean nativeVideoFitting(long inputImage, long shape, long frame);

}
