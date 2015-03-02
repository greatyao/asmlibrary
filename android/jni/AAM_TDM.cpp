/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#include "AAM_TDM.h"
#include "AAM_PAW.h"
#include "AAM_Util.h"

//============================================================================
AAM_TDM::AAM_TDM()
{
	__MeanTexture = 0;
	__TextureEigenVectors = 0;
    __TextureEigenValues = 0; 

}

//============================================================================
AAM_TDM::~AAM_TDM()
{
	cvReleaseMat(&__MeanTexture);
	cvReleaseMat(&__TextureEigenVectors);
	cvReleaseMat(&__TextureEigenValues);
}

//============================================================================
void AAM_TDM::Train(const file_lists& pts_files, const file_lists& img_files, 
					const AAM_PAW& m_warp, 
					float texture_percentage /* = 0.975 */, 
					bool registration /* = true */)
{
	int nPoints = m_warp.nPoints();
	int nPixels = m_warp.nPix()*3;
	int nSamples = pts_files.size();
	
	CvMat *AllTextures = cvCreateMat(nSamples, nPixels, CV_32FC1);
	
	CvMat * matshape = cvCreateMat(1, nPoints*2, CV_32FC1);
	for(int i = 0; i < nSamples; i++)
	{
		IplImage* image = cvLoadImage(img_files[i].c_str(), -1);
		
		AAM_Shape trueshape;
		if(!trueshape.ReadAnnotations(pts_files[i]))
			trueshape.ScaleXY(image->width, image->height);
		trueshape.Point2Mat(matshape);
		AAM_Common::CheckShape(matshape, image->width, image->height);
		
		CvMat t;	cvGetRow(AllTextures, &t, i);
		m_warp.CalcWarpTexture(matshape, image, &t);
		
		cvReleaseImage(&image);
	}
	cvReleaseMat(&matshape);
	
	// align texture so as to minimize the lighting variation
	AAM_TDM::AlignTextures(AllTextures);
	
	//now do pca
	DoPCA(AllTextures, texture_percentage);

	if(registration) SaveSeriesTemplate(AllTextures, m_warp);

	cvReleaseMat(&AllTextures);
}

//============================================================================
void AAM_TDM::DoPCA(const CvMat* AllTextures, float percentage)
{
	LOGD("Doing PCA of textures datas...");

	int nSamples = AllTextures->rows;
	int nPixels = AllTextures->cols;
    int nEigenAtMost = MIN(nSamples, nPixels);

    CvMat* tmpEigenValues = cvCreateMat(1, nEigenAtMost, CV_32FC1);
    CvMat* tmpEigenVectors = cvCreateMat(nEigenAtMost, nPixels, CV_32FC1);
    __MeanTexture = cvCreateMat(1, nPixels, CV_32FC1 );

    cvCalcPCA(AllTextures, __MeanTexture, 
        tmpEigenValues, tmpEigenVectors, CV_PCA_DATA_AS_ROW);

	float allSum = cvSum(tmpEigenValues).val[0];
	float partSum = 0.0;
    int nTruncated = 0;
    float largesteigval = cvmGet(tmpEigenValues, 0, 0);
	for(int i = 0; i < nEigenAtMost; i++)
    {
		float thiseigval = cvmGet(tmpEigenValues, 0, i);
        if(thiseigval / largesteigval < 0.0001) break; // firstly check(remove small values)
		partSum += thiseigval;
		++ nTruncated;
        if(partSum/allSum >= percentage)	break;    //secondly check
    }

	__TextureEigenValues = cvCreateMat(1, nTruncated, CV_32FC1);
	__TextureEigenVectors = cvCreateMat(nTruncated, nPixels, CV_32FC1);
    
	CvMat G;
	cvGetCols(tmpEigenValues, &G, 0, nTruncated);
	cvCopy(&G, __TextureEigenValues);

	cvGetRows(tmpEigenVectors, &G, 0, nTruncated);
	cvCopy(&G, __TextureEigenVectors);

	cvReleaseMat(&tmpEigenVectors);
	cvReleaseMat(&tmpEigenValues);

	LOGD("Done (%d/%d)\n", nTruncated, nEigenAtMost);
}

//============================================================================
void AAM_TDM::CalcTexture(const CvMat* lamda, CvMat* t)
{
	cvBackProjectPCA(lamda, __MeanTexture, __TextureEigenVectors, t);
}

//============================================================================
void AAM_TDM::CalcParams(const CvMat* t, CvMat* lamda)
{
	cvProjectPCA(t, __MeanTexture, __TextureEigenVectors, lamda);
}

//============================================================================
void AAM_TDM::Clamp(CvMat* lamda, float s_d /* = 3.0 */)
{
	float* fastp = lamda->data.fl;
	float* fastv = __TextureEigenValues->data.fl;
	int nmodes = nModes();
	float limit;

	for(int i = 0; i < nmodes; i++)
	{
		limit = s_d*sqrt(fastv[i]);
		if(fastp[i] > limit) fastp[i] = limit;
		else if(fastp[i] < -limit) fastp[i] = -limit;	
	}
}

//============================================================================
void AAM_TDM::AlignTextures(CvMat* AllTextures)
{
	LOGD("Align textures to minimize the lighting variation ...\n");
	
	int nsamples = AllTextures->rows;
	int npixels = AllTextures->cols;
	CvMat* meanTexture = cvCreateMat(1, npixels, CV_32FC1);
	CvMat* lastMeanEstimate = cvCreateMat(1, npixels, CV_32FC1);
	CvMat* constmeanTexture = cvCreateMat(1, npixels, CV_32FC1);
	CvMat ti;

	// calculate the mean texture 
	AAM_TDM::CalcMeanTexture(AllTextures, meanTexture);
	AAM_TDM::ZeroMeanUnitLength(meanTexture);
	cvCopy(meanTexture, constmeanTexture);
		
	// do a number of alignment iterations until convergence
    float diff, diff_max = 1e-6;
	const int max_iter = 15;
	for(int iter = 0; iter < max_iter; iter++)
	{
		cvCopy(meanTexture, lastMeanEstimate);
		//align all textures to the mean texture estimate
		for(int i = 0; i < nsamples; i++)
		{
			cvGetRow(AllTextures, &ti, i);
			AAM_TDM::NormalizeTexture(meanTexture, &ti);
		}

		//estimate new mean texture
		AAM_TDM::CalcMeanTexture(AllTextures, meanTexture);
		AAM_TDM::NormalizeTexture(constmeanTexture, meanTexture);
		
		// test if the mean estimate has converged
		diff = cvNorm(meanTexture, lastMeanEstimate);
		LOGD("\tAlignment iteration #%i, mean texture est. diff. = %g\n", iter, diff );
		if(diff <= diff_max) break;		
	}  

	cvReleaseMat(&meanTexture);
	cvReleaseMat(&lastMeanEstimate);
	cvReleaseMat(&constmeanTexture);
}

//============================================================================
void AAM_TDM::CalcMeanTexture(const CvMat* AllTextures, CvMat* meanTexture)
{
	CvMat submat;
	for(int i = 0; i < meanTexture->cols; i++)
	{
		cvGetCol(AllTextures, &submat, i);
		cvmSet(meanTexture, 0, i, cvAvg(&submat).val[0]);
	}
}


//============================================================================
void AAM_TDM::NormalizeTexture(const CvMat* refTextrure, CvMat* Texture)
{
	AAM_TDM::ZeroMeanUnitLength(Texture);
	float alpha = cvDotProduct(Texture, refTextrure);
	if(alpha != 0)	cvConvertScale(Texture, Texture, 1.0/alpha, 0);
}

//============================================================================
void AAM_TDM::ZeroMeanUnitLength(CvMat* Texture)
{
	CvScalar mean =  cvAvg(Texture);
	cvSubS(Texture, mean, Texture);
	float norm = cvNorm(Texture);
	cvConvertScale(Texture, Texture, 1.0/norm);
}

//============================================================================
void AAM_TDM::SaveSeriesTemplate(const CvMat* AllTextures, const AAM_PAW& m_warp)
{
	LOGD("Saving the face template image...\n");
	AAM_Common::MkDir("registration");
	AAM_Common::MkDir("Modes");
	AAM_Common::MkDir("Tri");
	char filename[100];
	
	int i;
	for(i = 0; i < AllTextures->rows; i++)
	{
		CvMat oneTexture;
		cvGetRow(AllTextures, &oneTexture, i);
		sprintf(filename, "registration/%d.jpg", i);
		m_warp.SaveWarpTextureToImage(filename, &oneTexture);
	}
	
	for(int nmodes = 0; nmodes < nModes(); nmodes++)
	{
		CvMat oneVar;
		cvGetRow(__TextureEigenVectors, &oneVar, nmodes);
	
		sprintf(filename, "Modes/A%03d.jpg", nmodes+1);
		m_warp.SaveWarpTextureToImage(filename, &oneVar);
	}
	
	IplImage* templateimg = cvCreateImage
		(cvSize(m_warp.Width(), m_warp.Height()), IPL_DEPTH_8U, 3);
	IplImage* convexImage = cvCreateImage
		(cvSize(m_warp.Width(), m_warp.Height()), IPL_DEPTH_8U, 3);
	IplImage* TriImage = cvCreateImage
		(cvSize(m_warp.Width(), m_warp.Height()), IPL_DEPTH_8U, 3);

	m_warp.SaveWarpTextureToImage("Modes/Template.jpg", __MeanTexture);
	m_warp.TextureToImage(templateimg, __MeanTexture);

	cvSetZero(convexImage);
	for(i = 0; i < m_warp.nTri(); i++)
	{
		CvPoint p, q;
		int ind1, ind2;

		cvCopy(templateimg, TriImage);

		ind1 = m_warp.Tri(i, 0); ind2 = m_warp.Tri(i, 1);
		p = cvPointFrom32f(m_warp.Vertex(ind1));
		q = cvPointFrom32f(m_warp.Vertex(ind2));
		cvLine(TriImage, p, q, CV_RGB(255, 255, 255));
		cvLine(convexImage, p, q, CV_RGB(255, 255, 255));

		ind1 = m_warp.Tri(i, 1); ind2 = m_warp.Tri(i, 2);
		p = cvPointFrom32f(m_warp.Vertex(ind1));
		q = cvPointFrom32f(m_warp.Vertex(ind2));
		cvLine(TriImage, p, q, CV_RGB(255, 255, 255));
		cvLine(convexImage, p, q, CV_RGB(255, 255, 255));

		ind1 = m_warp.Tri(i, 2); ind2 = m_warp.Tri(i, 0);
		p = cvPointFrom32f(m_warp.Vertex(ind1));
		q = cvPointFrom32f(m_warp.Vertex(ind2));
		cvLine(TriImage, p, q, CV_RGB(255, 255, 255));
		cvLine(convexImage, p, q, CV_RGB(255, 255, 255));

		sprintf(filename, "Tri/%03i.jpg", i+1);
		cvSaveImage(filename, TriImage);
	}
	cvSaveImage("Tri/convex.jpg", convexImage);
	
	cvReleaseImage(&templateimg);
	cvReleaseImage(&convexImage);
	cvReleaseImage(&TriImage);
}

//============================================================================
void AAM_TDM::Write(std::ofstream& os)
{
	int _npixels = nPixels();
	int _nModes = nModes();

	os.write((char*)&_npixels, sizeof(int));
	os.write((char*)&_nModes, sizeof(int));

	WriteCvMat(os, __MeanTexture);
	//WriteCvMat(os, __TextureEigenValues);
	//WriteCvMat(os, __TextureEigenVectors);
}

//============================================================================
void AAM_TDM::Read(std::ifstream& is)
{
	int _npixels, _nModes;
	is.read((char*)&_npixels, sizeof(int));
	is.read((char*)&_nModes, sizeof(int));
	
	__MeanTexture = cvCreateMat(1, _npixels, CV_32FC1);
	//__TextureEigenValues = cvCreateMat(1, _nModes, CV_32FC1);
	//__TextureEigenVectors = cvCreateMat(_nModes, _npixels, CV_32FC1);

	ReadCvMat(is, __MeanTexture);
	//ReadCvMat(is, __TextureEigenValues);
	//ReadCvMat(is, __TextureEigenVectors);
}
