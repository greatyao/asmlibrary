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
		double scale = 1.0)
	{	Train(pts_files, img_files, scale); }

	void Train(const file_lists& pts_files, const file_lists& img_files, 
		double scale = 1.0, double shape_percentage = 0.975, 
		double texture_percentage = 0.975);

	// Draw the image according the searching result(0:point, 1:triangle, 2:appearance)
	void DrawResult(IplImage* image, const AAM_Shape& Shape, int type);

	//void Draw(IplImage* drawimage, const CvMat* s, int type, const IplImage*image = 0);
	void Draw(IplImage* image, const AAM_Shape& Shape, 
		const AAM_Shape& RefShape, const IplImage* refImage = NULL);


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
	CvMat*		__update_s0;	/*shape change at the base mesh */
	CvMat*		__inv_pq;		/*inverse parameters at the base mesh*/
	
	CvMat*		__warp_t;		/*warp image to base mesh*/
	CvMat*		__error_t;		/*error between warp image and template image A0*/
	CvMat*		__search_pq;	/*search parameters */
	CvMat*		__delta_pq;		/*parameters change to be updated*/
	CvMat*		__current_s;		/*current search shape*/
	CvMat*		__update_s;		/*shape after composing the warp*/
	CvMat*		__lamda;		/*appearance parameters*/
};

#endif 
