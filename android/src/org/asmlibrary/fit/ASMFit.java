package org.asmlibrary.fit;

import org.opencv.core.Mat;

public class ASMFit {
	
	static{  
		// Load native library after(!) OpenCV initialization
        System.loadLibrary("asmlibrary");
        System.loadLibrary("jni-asmlibrary");
		}  
	
	public static native boolean nativeReadModel(String modelName);

	public static native boolean nativeReadAAMModel(String modelName);
	
	/**
	 * @param cascadeName could be haarcascade_frontalface_alt2.xml 
	 * @return
	 */
	public static native boolean nativeInitCascadeDetector(String cascadeName);
	public static native void nativeDestroyCascadeDetector();
	
	/**
	 * @param cascadeName could be lbpcascade_frontalface.xml 
	 * @return
	 */
	public static native boolean nativeInitFastCascadeDetector(String cascadeName);
	public static native void nativeDestroyFastCascadeDetector();

	/**
	 * This function can only be used after nativeInitCascadeDetector()
	 * @param imageGray original gray image 
	 * @param face all faces' feature points 
	 * @return true if found faces, false otherwise
	 */
	public static boolean detectAll(Mat imageGray, Mat faces){
		return nativeDetectAll(imageGray.getNativeObjAddr(), 
				faces.getNativeObjAddr());
	}
	
	/**
	 * This function can only be used after nativeInitFastCascadeDetector()
	 * @param imageGray original gray image 
	 * @param faces all faces' feature points 
	 * @return true if found faces, false otherwise
	 */
	public static boolean fastDetectAll(Mat imageGray, Mat faces){
		return nativeFastDetectAll(imageGray.getNativeObjAddr(), 
				faces.getNativeObjAddr());
	}
	
	/**
	 * This function can only be used after nativeInitCascadeDetector()
	 * @param imageGray original gray image 
	 * @param faces only one face's feature points 
	 * @return true if found faces, false otherwise
	 */
	public static boolean detectOne(Mat imageGray, Mat face){
		return nativeDetectOne(imageGray.getNativeObjAddr(), 
				face.getNativeObjAddr());
	}
	
	public static void initShape(Mat faces){
		nativeInitShape(faces.getNativeObjAddr());
	}
	
	
	public static void fitting(Mat imageGray, Mat shapes, long n_iteration){
		nativeFitting(imageGray.getNativeObjAddr(), 
				shapes.getNativeObjAddr(), n_iteration);
	}
	
	public static boolean videoFitting(Mat imageGray, Mat shape, long frame, long n_iteration){
		return nativeVideoFitting(imageGray.getNativeObjAddr(),
				shape.getNativeObjAddr(), frame, n_iteration);
	}
	
	public static void drawAvatar(Mat imageColor, Mat shape){
		nativeDrawAvatar(imageColor.getNativeObjAddr(), shape.getNativeObjAddr());
	}
	
	private static native boolean nativeDetectAll(long inputImage, long faces);
	private static native boolean nativeDetectOne(long inputImage, long face);
	private static native boolean nativeFastDetectAll(long inputImage, long faces);
	private static native void nativeInitShape(long faces);	
	private static native void nativeFitting(long inputImage, long shapes, long n_iteration);
	private static native boolean nativeVideoFitting(long inputImage, long shape, long frame, long n_iteration);
	private static native void nativeDrawAvatar(long inputImage, long shape);
}
