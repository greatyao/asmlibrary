/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#include "AAM_IC.h"
#include <fstream>
using namespace std;

//============================================================================
AAM_IC::AAM_IC()
{
	__Points = 0;
	__Storage = 0;

	__update_s0 = 0;
	__warp_t = 0;
	__error_t = 0;
	__search_pq = 0;
	__delta_pq = 0;
	__current_s = 0;
	__update_s = 0;
}

//============================================================================
AAM_IC::~AAM_IC()
{
	cvReleaseMat(&__Points);
	cvReleaseMemStorage(&__Storage);

	cvReleaseMat(&__update_s0);
	cvReleaseMat(&__warp_t);
	cvReleaseMat(&__error_t);
	cvReleaseMat(&__search_pq);
	cvReleaseMat(&__delta_pq);
	cvReleaseMat(&__current_s);
	cvReleaseMat(&__update_s);
}

//============================================================================
void AAM_IC::Train(const file_lists& pts_files, 
				   const file_lists& img_files, 
				   double scale /* = 1.0 */, 
				   double shape_percentage /* = 0.975 */, 
				   double texture_percentage /* = 0.975 */)
{
	if(pts_files.size() != img_files.size())
	{
		LOGW("ERROE(%s, %d): #Shapes != #Images\n", __FILE__, __LINE__);
		exit(0);
	}

	LOGD("################################################\n");
	LOGD("Build Inverse Compositional Image Alignmennt Model...\n");

	std::vector<AAM_Shape> AllShapes;
	for(int ii = 0; ii < pts_files.size(); ii++)
	{
		AAM_Shape Shape;
		bool flag = Shape.ReadAnnotations(pts_files[ii]);
		if(!flag)
		{
			IplImage* image = cvLoadImage(img_files[ii].c_str(), -1);
			Shape.ScaleXY(image->width, image->height);
			cvReleaseImage(&image);
		}
		AllShapes.push_back(Shape);
	}

	//building shape and texture distribution model
	LOGD("Build point distribution model...\n");
	__shape.Train(AllShapes, scale, shape_percentage);
	
	LOGD("Build warp information of mean shape mesh...");
	__Points = cvCreateMat (1, __shape.nPoints(), CV_32FC2);
	__Storage = cvCreateMemStorage(0);

	double sp = 1.0;
	//if(__shape.GetMeanShape().GetWidth() > 150)
	//	sp = 150/__shape.GetMeanShape().GetWidth();

	__paw.Train(__shape.GetMeanShape()*sp, __Points, __Storage);
	LOGD("[%d by %d, triangles #%d, pixels #%d*3]\n",
		__paw.Width(), __paw.Height(), __paw.nTri(), __paw.nPix());

	LOGD("Build texture distribution model...\n");
	__texture.Train(pts_files, img_files, __paw, texture_percentage, true);

	//alocate memory for on-line fitting stuff
	__update_s0 = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__inv_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__warp_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__error_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__search_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__delta_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__current_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__update_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__lamda  = cvCreateMat(1, __texture.nModes(), CV_64FC1);

	LOGD("################################################\n\n");
}

//============================================================================
void AAM_IC::DrawResult(IplImage* image, const AAM_Shape& Shape, int type)
{
	if(type == 0) AAM_Common::DrawPoints(image, Shape);
	else if(type == 1) AAM_Common::DrawTriangles(image, Shape, __paw.__tri);
	else if(type == 2) 
	{
		cvGEMM(__error_t, __texture.GetBases(), 1, NULL, 1, __lamda, CV_GEMM_B_T);
		__texture.CalcTexture(__lamda, __warp_t);
		AAM_PAW paw;
		double minV, maxV;
		cvMinMaxLoc(__warp_t, &minV, &maxV);
		cvConvertScale(__warp_t, __warp_t, 255/(maxV-minV), -minV*255/(maxV-minV));
		paw.Train(Shape, __Points, __Storage, __paw.GetTri(), false);
		AAM_Common::DrawAppearance(image, Shape, __warp_t, paw, __paw);
	}
	else LOGW("ERROR(%s, %d): Unsupported drawing type\n", __FILE__, __LINE__);
}

//============================================================================
void AAM_IC::Draw(IplImage* image, const AAM_Shape& Shape, 
		const AAM_Shape& RefShape, const IplImage* refImage, bool zero)
{
	//get the warped texture of another person
	if(refImage == NULL)
		cvCopy(__texture.GetMean(), __warp_t);
	else
	{
		RefShape.Point2Mat(__current_s);
		AAM_Common::CheckShape(__current_s, refImage->width, refImage->height);
		__paw.CalcWarpTexture(__current_s, refImage, __warp_t);
	}
		
	//draw the appearance
	Shape.Point2Mat(__current_s);
	AAM_Common::CheckShape(__current_s, image->width, image->height);

	double minV, maxV;
	cvMinMaxLoc(__warp_t, &minV, &maxV);
	cvConvertScale(__warp_t, __warp_t, 255/(maxV-minV), -minV*255/(maxV-minV));
	
	AAM_PAW paw;
	paw.Train(Shape, __Points, __Storage, __paw.GetTri(), false);

	if(zero) cvZero(image);
	AAM_Common::DrawAppearanceWithThread(image, Shape, __warp_t, paw, __paw, 2);
}

//============================================================================
void AAM_IC::Write(std::ofstream& os)
{
	__shape.Write(os);
	__texture.Write(os);
	__paw.Write(os);
	__sMean.Write(os);
}

//============================================================================
void AAM_IC::Read(std::ifstream& is)
{
	__shape.Read(is);
	__texture.Read(is);
	__paw.Read(is);

	int nPoints = __shape.nPoints();
	__sMean.resize(nPoints);
	__sMean.Read(is);

	//alocate memory for on-line fitting stuff
	__Points = cvCreateMat (1, __shape.nPoints(), CV_32FC2);
	__Storage = cvCreateMemStorage(0);

	__update_s0 = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__inv_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__warp_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__error_t = cvCreateMat(1, __texture.nPixels(), CV_64FC1);
	__search_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__delta_pq = cvCreateMat(1, __shape.nModes()+4, CV_64FC1);
	__current_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__update_s = cvCreateMat(1, __shape.nPoints()*2, CV_64FC1);
	__lamda  = cvCreateMat(1, __texture.nModes(), CV_64FC1);
}

//============================================================================
bool AAM_IC::WriteModel(const std::string& filename)
{
	ofstream os(filename.c_str(), ios::out | ios::binary);
	if(!os){
		LOGW("ERROR(%s, %d): CANNOT create model \"%s\"\n", __FILE__, __LINE__, filename.c_str());
		return false;
	}

	LOGD("Writing active appearance model to file...\n");
	Write(os);
	
	LOGD("Done\n");
	
	os.close();
	
	return true;
}

//============================================================================
bool AAM_IC::ReadModel(const std::string& filename)
{
	ifstream is(filename.c_str(), ios::in | ios::binary);
	if(!is){
		LOGW("ERROR(%s, %d): CANNOT load model \"%s\"\n", __FILE__, __LINE__, filename.c_str());
		return false;
	}

	LOGI("Reading active appearance model from file...\n");
	Read(is);
	LOGI("Done\n");

	is.close();
	
	return true;
}
