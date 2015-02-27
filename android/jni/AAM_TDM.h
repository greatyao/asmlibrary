/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#ifndef AAM_TDM_H
#define AAM_TDM_H

#include "AAM_Util.h"
#include "AAM_Shape.h"
#include "AAM_PAW.h"

class AAM_CAM;

//Texture distribution model.
class AAM_TDM  
{
	friend class AAM_CAM;
public:
	AAM_TDM();
	~AAM_TDM();

	//build texture distribution model
	void Train(const file_lists& pts_files, const file_lists& img_files,
		const AAM_PAW& m_warp, double texture_percentage = 0.975, 
		bool registration  = true);

	// Read data from stream 
	void Read(std::ifstream& is);

	// write data to stream
	void Write(std::ofstream& os);

	//calculate texture according to parameters lamda
	void CalcTexture(const CvMat* lamda, CvMat* t);
	
	//calculate parameters lamda according to texture sample 
	void CalcParams(const CvMat* t, CvMat* lamda);

	//Limit texture parameters.
    void Clamp(CvMat* lamda, double s_d = 3.0);

	// Normalize texture make sure: sum of element is o and variance is 1 
	static void ZeroMeanUnitLength(CvMat* Texture);

	// Normailize texture to mean texture
	static void NormalizeTexture(const CvMat* refTextrure, CvMat* Texture);

	// Get number of color-pixels in texture model 
	inline const int nPixels()const{return __MeanTexture->cols;}

	// Get number of modes of texture variation
	inline const int nModes()const{return __TextureEigenVectors->rows;}

	// Get mean texture
	inline const CvMat* GetMean()const{ return __MeanTexture;	}

	// Get texture eigen-vectors of PCA (modes modes)
	inline const CvMat* GetBases()const{ return __TextureEigenVectors;	}

	inline const double Var(int i)const{ return cvmGet(__TextureEigenValues,0,i); }

private:

	// Do pca of texture data
	void DoPCA(const CvMat* AllTextures, double percentage);

	// Align texture to lossen the affect of light variations
	static void AlignTextures(CvMat* AllTextures);

	// Calculate mean texture
	static void CalcMeanTexture(const CvMat* AllTextures, CvMat* meanTexture); 

	// Save the raw texture 
	void SaveSeriesTemplate(const CvMat* AllTextures, const AAM_PAW& m_warp);

private:
	
	CvMat*  __MeanTexture;
	CvMat*  __TextureEigenVectors;
    CvMat*  __TextureEigenValues; 
};

#endif // 
