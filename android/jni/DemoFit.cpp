#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include "asmfitting.h"
#include "vjfacedetect.h"
#include "AAM_IC.h"

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
AAM_IC aam;
DetectionBasedTracker *track = NULL;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeReadAAMModel
(JNIEnv * jenv, jclass, jstring jFileName)
{
    LOGD("nativeReadAAMModel enter");
    const char* filename = jenv->GetStringUTFChars(jFileName, NULL);
    jboolean result = false;

    try
    {
	if(aam.ReadModel(filename) == true)
		result = true;

    }
    catch (...)
    {
        LOGD("nativeReadAAMModel caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code");
    }

    LOGD("nativeReadAAMModel %s exit %d", filename, result);
    return result;
}


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

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeInitFastCascadeDetector
(JNIEnv * jenv, jclass, jstring jFileName)
{
    const char* cascade_name = jenv->GetStringUTFChars(jFileName, NULL);
    LOGD("nativeInitFastCascadeDetector %s enter", cascade_name);

    DetectionBasedTracker::Parameters DetectorParams;
    DetectorParams.minObjectSize = 45;
    track = new DetectionBasedTracker(cascade_name, DetectorParams);

    if(track == NULL)	return false;

    DetectorParams = track->getParameters();
    DetectorParams.minObjectSize = 64;
    track->setParameters(DetectorParams);

    track->run();

    LOGD("nativeInitFastCascadeDetector exit");
    return true;
}

JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDestroyCascadeDetector
(JNIEnv * jenv, jclass)
{
    LOGD("nativeDestroyCascadeDetector enter");

    destory_detect_cascade();

    LOGD("nativeDestroyCascadeDetector exit");
}

JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDestroyFastCascadeDetector
(JNIEnv * jenv, jclass)
{
    LOGD("nativeDestroyFastCascadeDetector enter");

    if(track){
    	track->stop();
    	delete track;
    }

    LOGD("nativeDestroyFastCascadeDetector exit");
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

inline void Mat_to_shape(asm_shape shapes[], int nShape, Mat& mat)
{
	for(int i = 0; i < nShape; i++)
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

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeFastDetectAll
(JNIEnv * jenv, jclass, jlong imageGray, jlong faces)
{
	if(!track)	return false;

	BEGINT();

	vector<Rect> RectFaces;
	try{
		Mat image = *(Mat*)imageGray;
		LOGD("image: (%d, %d)", image.cols, image.rows);
		track->process(image);
		track->getObjects(RectFaces);
	}
	catch(cv::Exception& e)
	{
		LOGD("nativeFastDetectAll caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if(!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	}
	catch (...)
	{
		LOGD("nativeFastDetectAll caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, "Unknown exception in JNI code");
	}

	int nFaces = RectFaces.size();
	if(nFaces <= 0){
		ENDT("FastCascadeDetector CANNOT detect any face");
		return false;
	}

	LOGD("FastCascadeDetector found %d faces", nFaces);

	asm_shape* detshapes = new asm_shape[nFaces];
	for(int i = 0; i < nFaces; i++){
		Rect r = RectFaces[i];
		detshapes[i].Resize(2);
		detshapes[i][0].x = r.x;
		detshapes[i][0].y = r.y;
		detshapes[i][1].x = r.x+r.width;
		detshapes[i][1].y = r.y+r.height;
	}

	asm_shape* shapes = new asm_shape[nFaces];
	for(int i = 0; i < nFaces; i++)
	{
		InitShapeFromDetBox(shapes[i], detshapes[i], fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
	}

	shape_to_Mat(shapes, nFaces, *((Mat*)faces));

	delete []detshapes;
	delete []shapes;

	ENDT("FastCascadeDetector detect");

	return true;
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDetectAll
(JNIEnv * jenv, jclass, jlong imageGray, jlong faces)
{
	IplImage image = *(Mat*)imageGray;
	int nFaces;
	asm_shape *detshapes = NULL;

	LOGD("image: (%d, %d)", image.width, image.height);

	BEGINT();

	bool flag =detect_all_faces(&detshapes, nFaces, &image);
	if(flag == false)	{
		ENDT("CascadeDetector CANNOT detect any face");
		return false;
	}

	LOGD("CascadeDetector found %d faces", nFaces);
	asm_shape* shapes = new asm_shape[nFaces];
	for(int i = 0; i < nFaces; i++)
	{
		InitShapeFromDetBox(shapes[i], detshapes[i], fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
	}

	shape_to_Mat(shapes, nFaces, *((Mat*)faces));
	free_shape_memeory(&detshapes);	
	delete []shapes;
	
	ENDT("CascadeDetector detect");

	return true;
}

JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeInitShape(JNIEnv * jenv, jclass, jlong faces)
{
	Mat faces1 = *((Mat*)faces);
	int nFaces = faces1.rows;
	asm_shape* detshapes = new asm_shape[nFaces];
	asm_shape* shapes = new asm_shape[nFaces];

	Mat_to_shape(detshapes, nFaces, faces1);

	for(int i = 0; i < nFaces; i++)
	{
		InitShapeFromDetBox(shapes[i], detshapes[i], fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());
	}

	shape_to_Mat(shapes, nFaces, *((Mat*)faces));

	delete []detshapes;
	delete []shapes;
}


JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDetectOne
(JNIEnv * jenv, jclass, jlong imageGray, jlong faces)
{
	IplImage image = *(Mat*)imageGray;
	asm_shape shape, detshape;
	
	BEGINT();

	bool flag = detect_one_face(detshape, &image);

	if(flag == false)	{
		ENDT("CascadeDetector CANNOT detect any face");
		return false;
	}

	InitShapeFromDetBox(shape, detshape, fit_asm.GetMappingDetShape(), fit_asm.GetMeanFaceWidth());

	shape_to_Mat(&shape, 1, *((Mat*)faces));
	
	ENDT("CascadeDetector detects central face");

	return true;
}


JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeFitting
(JNIEnv * jenv, jclass, jlong imageGray, jlong shapes0, jlong n_iteration)
{
	IplImage image = *(Mat*)imageGray;
	Mat shapes1 = *(Mat*)shapes0;	
	int nFaces = shapes1.rows;	
	asm_shape* shapes = new asm_shape[nFaces];
	
	BEGINT();

	Mat_to_shape(shapes, nFaces, shapes1);

	fit_asm.Fitting2(shapes, nFaces, &image, n_iteration);

	shape_to_Mat(shapes, nFaces, *((Mat*)shapes0));

	ENDT("nativeFitting");

	//for(int i = 0; i < shapes[0].NPoints(); i++)
	//	LOGD("points: (%f, %f)", shapes[0][i].x, shapes[0][i].y);

	delete []shapes;
}

JNIEXPORT jboolean JNICALL Java_org_asmlibrary_fit_ASMFit_nativeVideoFitting
(JNIEnv * jenv, jclass, jlong imageGray, jlong shapes0, jlong frame, jlong n_iteration)
{
	IplImage image = *(Mat*)imageGray;
	Mat shapes1 = *(Mat*)shapes0;	
	bool flag = false;
	if(shapes1.rows == 1)
	{
		asm_shape shape;
	
		BEGINT();

		Mat_to_shape(&shape, 1, shapes1);

		flag = fit_asm.ASMSeqSearch(shape, &image, frame, false, n_iteration);

		shape_to_Mat(&shape, 1, *((Mat*)shapes0));

		ENDT("nativeVideoFitting");
	}

	return flag;
}

static AAM_Shape ShapeAAMFromASM(const asm_shape& shape)
{
	AAM_Shape s;
	s.resize(shape.NPoints());
	for(int i = 0; i < shape.NPoints(); i++)
	{
		s[i].x = shape[i].x;
		s[i].y = shape[i].y;
	}
	return s;
}

JNIEXPORT void JNICALL Java_org_asmlibrary_fit_ASMFit_nativeDrawAvatar
(JNIEnv * jenv, jclass, jlong imageColor, jlong shapes0)
{
	IplImage image = *(Mat*)imageColor;
	Mat shapes1 = *(Mat*)shapes0;	
	if(shapes1.rows == 1)
	{
		asm_shape shape;
	
		BEGINT();

		Mat_to_shape(&shape, 1, shapes1);

		aam.Draw(&image, ShapeAAMFromASM(shape),  AAM_Shape());

		ENDT("nativeDrawAvatar");
	}
}


#ifdef __cplusplus
}
#endif

