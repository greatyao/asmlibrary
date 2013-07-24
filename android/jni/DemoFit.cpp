#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "asmfitting.h"
#include "vjfacedetect.h"

#include <string>
#include <vector>

#include <android/log.h>
#include <jni.h>

using namespace std;
using namespace cv;

#define LOG_TAG "ASMLIBRARY"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

#define BEGINT()	double t = (double)cvGetTickCount();
#define ENDT(exp)	t = ((double)cvGetTickCount() -  t )/  (cvGetTickFrequency()*1000.);	\
					LOGD(exp " time cost: %.2f millisec\n", t);

asmfitting fit_asm;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeReadModel
(JNIEnv * jenv, jclass, jstring jFileName)
{
    LOGD("nativeReadModel enter");
    const char* filename = jenv->GetStringUTFChars(jFileName, NULL);
    jboolean result = false;

    try
    {
	if(fit_asm.Read(filename) == true)
		result = true;

    }
    catch (...)
    {
        LOGD("nativeReadModel caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code");
    }

    LOGD("nativeReadModel %s exit %d", filename, result);
    return result;
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeInitCascadeDetector
(JNIEnv * jenv, jclass, jstring jFileName)
{
    const char* cascade_name = jenv->GetStringUTFChars(jFileName, NULL);
    LOGD("nativeInitCascadeDetector %s enter", cascade_name);

    if(init_detect_cascade(cascade_name) == false)
		return false;

    LOGD("nativeInitCascadeDetector exit");
    return true;
}

JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDestroyCascadeDetector
(JNIEnv * jenv, jclass)
{
    LOGD("nativeDestroyCascadeDetector enter");

    destory_detect_cascade();

    LOGD("nativeDestroyCascadeDetector exit");
}

inline void shape_to_Mat(asm_shape shapes[], int nShape, Mat& mat)
{
	mat = Mat(nShape, shapes[0].NPoints()*2, CV_64FC1); 

	for(int i = 0; i < nShape; i++)
	{
		double *pt = mat.ptr<double>(i);  
		for(int j = 0; j < mat.cols/2; j++)
		{
			pt[2*j] = shapes[i][j].x;
			pt[2*j+1] = shapes[i][j].y;		
		}
	}
}

inline void Mat_to_shape(asm_shape shapes[], Mat& mat)
{
	for(int i = 0; i < mat.rows; i++)
	{
		double *pt = mat.ptr<double>(i);  
		shapes[i].Resize(mat.cols/2);
		for(int j = 0; j < mat.cols/2; j++)
		{
			shapes[i][j].x = pt[2*j];
			shapes[i][j].y = pt[2*j+1];
		}
	}
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDetectAll
(JNIEnv * jenv, jclass, jlong imageGray, jlong faces)
{
	LOGD("nativeDetectAll enter");
 
    IplImage image = *(Mat*)imageGray;
	int nFaces;
	asm_shape *detshapes = NULL;

	BEGINT();

	bool flag =detect_all_faces(&detshapes, nFaces, &image);
	if(flag == false)	return false;

	LOGD("nativeDetectAll found %d faces", nFaces);
	asm_shape* shapes = new asm_shape[nFaces];
	for(int i = 0; i < nFaces; i++)
	{
		InitShapeFromDetBox(shapes[i], detshapes[i], fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
	}

	shape_to_Mat(shapes, nFaces, *((Mat*)faces));
	free_shape_memeory(&detshapes);	
	delete []shapes;
	
	ENDT("Haar-like cascade detect");

	LOGD("nativeDetectAll exit");
	return true;
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDetectOne
(JNIEnv * jenv, jclass, jlong imageGray, jlong faces)
{
	LOGD("nativeDetectOne enter");
 
	IplImage image = *(Mat*)imageGray;
	asm_shape shape, detshape;
	
	BEGINT();

	bool flag = detect_one_face(detshape, &image);

	if(flag == false)	return false;

	InitShapeFromDetBox(shape, detshape, fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());

	shape_to_Mat(&shape, 1, *((Mat*)faces));
	
	ENDT("Haar-like cascade detect");

	LOGD("nativeDetectOne exit");
	return true;
}


JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeFitting
(JNIEnv * jenv, jclass, jlong imageGray, jlong shapes0)
{
	IplImage image = *(Mat*)imageGray;
	Mat shapes1 = *(Mat*)shapes0;	
	int nFaces = shapes1.rows;	
	asm_shape* shapes = new asm_shape[nFaces];
	
	BEGINT();

	Mat_to_shape(shapes, shapes1);

	fit_asm.Fitting2(shapes, nFaces, &image);

	shape_to_Mat(shapes, nFaces, *((Mat*)shapes0));

	ENDT("nativeFitting");

	//for(int i = 0; i < shapes[0].NPoints(); i++)
	//	LOGD("points: (%f, %f)", shapes[0][i].x, shapes[0][i].y);

	delete []shapes;
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeVideoFitting
(JNIEnv * jenv, jclass, jlong imageGray, jlong shapes0, jlong frame)
{
	IplImage image = *(Mat*)imageGray;
	Mat shapes1 = *(Mat*)shapes0;	
	bool flag = false;
	if(shapes1.rows == 1)
	{
		asm_shape shape;
	
		BEGINT();

		Mat_to_shape(&shape, shapes1);

		flag = fit_asm.ASMSeqSearch(shape, &image, frame, true);

		shape_to_Mat(&shape, 1, *((Mat*)shapes0));

		ENDT("nativeVideoFitting");
	}

	return flag;
}

#ifdef __cplusplus
}
#endif

