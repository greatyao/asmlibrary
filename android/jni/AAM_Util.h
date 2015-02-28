/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#ifndef AAM_UTIL_H
#define AAM_UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

//#include "cv.h"
//#include "highgui.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2//highgui/highgui_c.h>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>

#include "AAM_Shape.h"

#ifndef byte
typedef unsigned char byte;
#endif

#define gettime cvGetTickCount() / (cvGetTickFrequency()*1000.)

#ifdef ANDROID
    #include <android/log.h>
    #define LOG_TAG "AAMLIBRARY"
    #define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#else
	#define LOGD(...) fprintf(stdout,  __VA_ARGS__)
	#define LOGI(...) fprintf(stdout,  __VA_ARGS__)
	#define LOGW(...) fprintf(stderr,  __VA_ARGS__)
#endif

typedef std::vector<std::string> file_lists;

void ReadCvMat(std::istream &is, CvMat* mat);

void WriteCvMat(std::ostream &os, const CvMat* mat);


class AAM_PAW;

class AAM_Common
{
public:
	static file_lists ScanNSortDirectory(const std::string &path, 
		const std::string &extension);

	// Is the current shape within the image boundary?
	static void CheckShape(CvMat* s, int w, int h);

	static void DrawPoints(IplImage* image, const AAM_Shape& Shape);

	static void DrawTriangles(IplImage* image, const AAM_Shape& Shape, 
		const std::vector<std::vector<int> >&tris);
	
	static void DrawAppearance(IplImage*image, const AAM_Shape& Shape,
		const CvMat* t, const AAM_PAW& paw, const AAM_PAW& refpaw);

	static void DrawAppearanceWithThread(IplImage*image, const AAM_Shape& Shape,
		const CvMat* t, const AAM_PAW& paw, const AAM_PAW& refpaw, int thread = 2);
		
	static void* ThreadDrawAppearance(void* p);

	static int MkDir(const char* dirname);

};

#endif // AAM_UTIL_H
