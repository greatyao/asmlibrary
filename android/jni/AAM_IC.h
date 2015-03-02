/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/


#ifndef AAM_IC_H
#define AAM_IC_H

#include "AAM_Util.h"
#include "AAM_Shape.h"
#include "AAM_PDM.h"
#include "AAM_TDM.h"
#include "AAM_PAW.h"

/** 
   Active appearace model using project-out inverse compositional fitting method.
   Refer to: I. Matthews and S. Baker. "Active Appearance Models Revisited". 2004
*/
class AAM_IC 
{
public:
	AAM_IC();
	~AAM_IC();

	// Build aam inverse compositional model
	void Build(const file_lists& pts_files, const file_lists& img_files, 
		float scale = 1.0)
	{	Train(pts_files, img_files, scale); }

	void Train(const file_lists& pts_files, const file_lists& img_files, 
		float scale = 1.0, float shape_percentage = 0.975, 
		float texture_percentage = 0.975);

	void Draw(IplImage* image, const AAM_Shape& Shape, 
		const AAM_Shape& RefShape, const IplImage* refImage = NULL, bool zero = true);


	bool ReadModel(const std::string& filename);

	bool WriteModel(const std::string& filename);

	// Get Mean Shape of IC model
	inline const AAM_Shape GetMeanShape()const{ return __sMean;	}
	const AAM_Shape GetReferenceShape()const{ return __paw.__referenceshape;	}


private:
	// Read data from stream 
	void Read(std::ifstream& is);

	// Write data to stream
	void Write(std::ofstream& os);

private:

	//these variables are used for train PAW
	CvMat*			__Points;
	CvMemStorage*	__Storage;

private:
	
	AAM_PDM		__shape;		/*shape distribution model*/
	AAM_TDM		__texture;		/*shape distribution model*/
	AAM_PAW		__paw;			/*piecewise affine warp*/
	AAM_Shape	__sMean;		/*mean shape of model*/

private:
	//pre-allocated stuff for online alignment
	CvMat*		__warp_t;		/*warp image to base mesh*/
	CvMat*		__current_s;		/*current search shape*/
};

#endif 
