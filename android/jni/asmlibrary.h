/*
 * ASMLibrary(Active Shape Model Library) -- A compact SDK for face alignment
 *
 * Copyright (c) 2008-2011, Yao Wei <njustyw@gmail.com>,
 * http://visionopen.com/members/yaowei/, All rights reserved.
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */


/**@file 
Functions, structures, classes for implementing active shape model.

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#ifndef _ASM_LIBRARY_H_
#define _ASM_LIBRARY_H_

#include <stdio.h>

class asm_shape;
class asm_profile;
class asm_model;
struct profile_Nd_model;
struct profile_lbp_model;
struct CvMat;
struct _IplImage;

typedef unsigned char uchar;
typedef struct _IplImage IplImage;

#ifdef WIN32
#ifdef ASMLIBRARY_EXPORTS
#define ASMLIB __declspec(dllexport)
#else
#define ASMLIB __declspec(dllimport)
#endif
#else
#define ASMLIB
#endif

/**
 * Predefined local texture (profile) types.
 * <ul>
 * <li>PROFILE_1D: use only the pixels along the normal vector in the contour.</li>
 * <li>PROFILE_2D: use the pixels located at the recentage.</li>
 * <li>PROFILE_LBP: use the pixels processed with LBP-operator.</li>
 * </ul>
 **/
enum ASM_PROFILE_TYPE {PROFILE_1D, PROFILE_2D, PROFILE_LBP};

#ifdef __cplusplus
extern "C"{
#endif

/**
 Initialize shape from the detected box.
 @param shape the returned initial shape
 @param det_shape the detected box calling by \a asm_vjfacedetect::\a Detect()
 @param ref_shape the average mean shape
 @param refwidth the width of average mean shape
*/
ASMLIB void InitShapeFromDetBox(asm_shape &shape, const asm_shape& det_shape, 
								const asm_shape &ref_shape, double refwidth);

#ifdef __cplusplus
}
#endif

/** Class for 2d point. */
typedef struct Point2D32f
{
    float x;
    float y;
}
Point2D32f;

/** Class for 2d shape data. */
class ASMLIB asm_shape
{
public:
    /** Constructor */
	asm_shape();
    
	/** Copy Constructor */
	asm_shape(const asm_shape &v);
    
	/** Destructor */
    ~asm_shape();

	/**
     Access elements by \a CvPoint2D32f \a pt = \a shape[\a i] to get \a i-th point in the shape.
     @param i Index of points
     @return   Point at the certain index
	*/
	const Point2D32f operator [](int i)const{ return m_vPoints[i];	}
	
	/**
     Access elements by \a CvPoint2D32f \a pt = \a shape[\a i] to get \a i-th point in the shape.
     @param i Index of points
     @return   Point at the certain index
	*/
	Point2D32f& operator [](int i){ return m_vPoints[i];	}
	
	/**
     Get the number of points.
     @return   Number of points
	*/
	inline const int NPoints()const{ return	m_nPoints; }

    /**
     Override of operator =
    */
    asm_shape&			operator =(const asm_shape &s);
    
	/**
     Override of operator =.
    */
	asm_shape&			operator =(double value);
    
	/**
     Override of operator +
    */
    const asm_shape		operator +(const asm_shape &s)const;
    
	/**
     Override of operator +=
    */
    asm_shape&			operator +=(const asm_shape &s);
    
	/**
     Override of operator -
    */
    const asm_shape     operator -(const asm_shape &s)const;
    
	/**
     Override of operator -=
    */
    asm_shape&			operator -=(const asm_shape &s);
    
	/**
     Override of operator *
    */
    const asm_shape     operator *(double value)const;
    
	/**
     Override of operator *=
    */
    asm_shape&			operator *=(double value);
    
	/**
     Override of operator *
    */
    double				operator *(const asm_shape &s)const;
    
	/**
     Override of operator /
    */
    const asm_shape     operator /(double value)const;
    
	/**
     Override of operator /=
    */
    asm_shape&			operator /=(double value);

	/**
     Release memory.
    */
    void    Clear();
    
	/**
     Allocate memory.
	 @param length Number of of shape points
    */
    void    Resize(int length);
    
	/**
     Read points from file.
	 @param filename the filename the stored shape data
     @return   true on pts format, false on asf format, exit otherwise
    */
    bool	ReadAnnotations(const char* filename);
	
	/**
     Read points from asf format file.
	 @param filename the filename the stored shape data
    */
    void    ReadFromASF(const char*filename);
	
	/**
     Read points from pts format file.
	 @param filename the filename the stored shape data
    */
    void	ReadFromPTS(const char*filename);
	
	/**
     Write shape data into file stream.
	 @param f  stream to write to
    */
	void	Write(FILE* f);
	
	/**
     Read shape data from file stream.
	 @param f  stream to read from
    */
	void	Read(FILE* f);
	
	/**
     Calculate minimum \f$x\f$-direction value of shape.
    */
	const double  MinX()const;
    
	/**
     Calculate minimum \f$y\f$-direction value of shape.
    */
	const double  MinY()const;
    
	/**
     Calculate maximum \f$x\f$-direction value of shape.
    */
	const double  MaxX()const;
    
	/**
     Calculate maximum \f$y\f$-direction value of shape.
    */
	const double  MaxY()const;
	
	/**
     Calculate the left and right index for \f$x\f$-direction in the shape.
	 @param ileft the index of points in \f$x\f$-direction which has the minimum x
	 @param iright the index of points in \f$x\f$-direction which has the maximum x
    */
	void		  GetLeftRight(int& ileft, int& iright)const;
    
	/**
     Calculate width of shape.
	 @param ileft Index of points in \f$x\f$-direction which has the minimum x
	 @param iright Index of points in \f$x\f$-direction which has the maximum x
    */
	const double  GetWidth(int ileft = -1, int iright = -1)const;
	
	/**
     Calculate height of shape.
    */
	const double  GetHeight()const { return MaxY()-MinY();	}
	
    /**
     Calculate center of gravity for shape.
	 @param x Value of center in \f$x\f$-direction
	 @param y Value of center in \f$y\f$-direction
    */
	void    COG(double &x, double &y)const;
    
	/**
     Translate the shape to make its center locate at (0, 0).
	*/
	void    Centralize();
    
	/**
	 Translate the shape.
	 @param x Value of translation factor in \f$x\f$-direction
	 @param y Value of translation factor in \f$y\f$-direction
    */
	void    Translate(double x, double y);
    
	/**
     Scale shape by an uniform factor.
	 @param s Scaling factor
	*/
	void    Scale(double s);
    
	/**
     Rotate shape by anti clock-wise.
	 @param theta Angle to be rotated
	*/
	void    Rotate(double theta);
	
	/**
     Scale shape in x and y direction respectively.
	 @param sx Scaling factor in \f$x\f$-direction
	 @param sy Scaling factor in \f$y\f$-direction
	*/
	void    ScaleXY(double sx, double sy);
	
	/**
     Normalize shape (zero_mean_unit_length) so that its center locates at (0, 0) and its \f$L2\f$-norm is 1.
	 @return the \f$L2\f$-norm of original shape
	*/
	double	Normalize();
	
	
	enum{ LU, SVD, Direct };

	/**
	 Calculate the similarity transform between one shape and another reference shape. 
	 Where the similarity transform is: 
	 <BR>
	 \f$T(a,b,tx,ty) = [a \ -b \ Tx; b \ a \ Ty ; 0 \ 0 \ 1]\f$.
	 @param ref_shape the reference shape
	 @param a  will return \f$ s \times cos(theta) \f$ in form of similarity transform
	 @param b  will return \f$ s \times sin(theta) \f$ in form of similarity transform
	 @param tx will return \f$ Tx \f$ in form of similarity transform
	 @param ty will return \f$ Ty \f$ in form of similarity transform
	 @param method  Method of similarity transform
	*/
	void    AlignTransformation(const asm_shape &ref_shape, double &a, double &b, 
								double &tx, double &ty, int method = SVD)const;
    
	/**
	 Align the shape to the reference shape. 
	 @param ref_shape the reference shape
	 @param method  method of similarity transform
	*/
	void    AlignTo(const asm_shape &ref_shape, int method = SVD);
    
	/**
	 Transform Shape using the similarity transform \f$T(a,b,tx,ty)\f$. 
	*/
	void    TransformPose(double a, double b, double tx, double ty);

	/**
	 Calculate the angular bisector between two lines \f$Pi-Pj\f$ and \f$Pj-Pk\f$. 
	 @param i the index of point vertex
	 @param j the index of point vertex
	 @param k the index of point vertex
	 @return Angular bisector vector in form of \f$(cos(x), sin(x))^T\f$
	*/
	Point2D32f CalcBisector(int i, int j, int k)const;

	/**
	 Calculate the Euclidean norm (\f$L2\f$-norm). 
	 @return Euclidean norm
	*/
	double  GetNorm2()const;

	/**
	 Calculate the normal vector at certain vertex around the shape contour. 
	 @param cos_alpha the normal vector in \f$x\f$-direction
	 @param sin_alpha the normal vector in \f$y\f$-direction
	 @param i the index of point vertex
	*/
	void	CalcNormalVector(double &cos_alpha, double &sin_alpha, int i)const;

	/**
	 Convert from OpenCV's \a CvMat to class asm_shape
	 @param mat \a CvMat that converted from
	*/
	void    CopyFrom(const CvMat* mat);
	
	/**
	 Convert from class asm_shape to OpenCV's CvMat.
	 @param mat CvMat that converted to
	*/
	void    CopyTo(CvMat* mat)const;

private:
	void    Transform(double c00, double c01, double c10, double c11);

private:
	Point2D32f* m_vPoints;	/**< point data */
	int m_nPoints;				/**< number of points */
};

/** Left and Right index in \f$x\f$-direction of shape */
typedef struct scale_param
{
	int left;	/**< Index of points in \f$x\f$-direction which has the minimum x */
	int right;	/**< Index of points in \f$x\f$-direction which has the maximum x */
}scale_param;


/** Class for active shape model. */
class ASMLIB asm_model
{
public:
	/**
	 Constructor
	*/
	asm_model();
	
	/**
	 Destructor
	*/
	~asm_model();

	/**
	 Image alignment/fitting with an initial shape.
	 @param shape the point features that carries initial shape and also restores result after fitting
	 @param grayimage the gray image resource
	 @param max_iter the number of iteration
	 @param param the left and right index for \f$x\f$-direction in the shape (Always set \a NULL )
	 @return false on failure, true otherwise
	*/
	bool Fit(asm_shape& shape, const IplImage *grayimage, 
		int max_iter = 30, const scale_param* param = NULL);	
	
	/**
     Write model data to file stream.
	 @param f  stream to write to
    */
	void WriteModel(FILE* f);
	
	/**
     Read model data from file stream.
	 @param f  stream to read from
    */
	void ReadModel(FILE* f);

	/**
	 Get mean shape of model.
	*/
	const asm_shape& GetMeanShape()const { return m_asm_meanshape;	}
	
	/**
	 Get modes of shape distribution model (Will be calculated in shape's PCA)
	*/
	const int GetModesOfModel()const { return m_nModes;}
	
	/**
	 Get the width of mean shape [Identical to \a m_asm_meanshape.\a GetWidth()].
	*/
	const double GetReferenceWidthOfFace()const { return m_dReferenceFaceWidth; }

private:

	/**
     Get the optimal offset at one certain point vertex during the process of 
	 best profile matching (work for 1d/2d profile model).
	 @param image the image resource
	 @param ilev one certain pyramid level
	 @param shape the shape data
	 @param ipoint the index of point vertex
	 @param cos_alpha the normal vector in \f$x\f$-direction
	 @param sin_alpha the normal vector in \f$y\f$-direction
	 @return offset bias from \a Shape[\a iPoint]
	*/
	int FindBestOffsetForNd(const IplImage* image, int ilev,
							const asm_shape& shape, int ipoint,
							double& cos_alpha, double& sin_alpha);

	/**
     Get the optimal offset at one certain point vertex during the process of 
	 best profile matching (work for lbp profile model).
  	 @param lbp_img the target image processed with LBP
	 @param nrows the height of \a lbp_img
	 @param ncols the width of \a lbp_img
	 @param ilev one certain pyramid level
	 @param shape the shape data
	 @param ipoint the index of point vertex
	 @param xoffset the returned offset in \f$x\f$-direction away from \a Shape[\a iPoint]
	 @param yoffset the returned offset in \f$y\f$-direction away from \a Shape[\a iPoint]
	*/
	void FindBestOffsetForLBP(const int* lbp_img, int nrows, int ncols, int ilev,
				const asm_shape& shape, int ipoint, int& xoffset, int& yoffset);

	/**
     Update shape by matching the image profile to the model profile.
	 @param update_shape the updated shape
	 @param shape  the point feature that will be matched
	 @param ilev one certain pyramid level
	 @param image the image resource
	 @param lbp_img the LBP-operator image
	 @param norm the \f$L2\f$-norm of the difference between \a shape and \a update_shape
	 @return how many point vertex will be updated?
	*/
	int MatchToModel(asm_shape& update_shape, const asm_shape& shape, 
		int ilev, const IplImage* image, const int *lbp_img, double* norm = NULL);

	/**
     Calculate shape parameters (\a a, \a b, \a Tx, \a Ty) and pose parameters \a p.
	 @param p  Shape parameters
	 @param a  \f$ s \times cos(theta) \f$ in form of similarity transform
	 @param b  \f$ s \times sin(theta) \f$ in form of similarity transform
	 @param tx \f$ Tx \f$ in form of similarity transform
	 @param ty \f$ Ty \f$ in form of similarity transform
	 @param shape  the point features data
	 @param iter_no Number of iteration
	*/
	void CalcParams(CvMat* p, double& a, double& b,	double& tx, double& ty, 
		const asm_shape& shape, int iter_no = 2);
	
	/**
     Constrain the shape parameters.
	 @param p  Shape parameters
	*/
	void Clamp(CvMat* p);

	/**
     Generate shape instance according to shape parameters p and pose parameters.
	 @param shape the point feature data
	 @param p  the shape parameters
	 @param a  Return \f$ s \times cos(theta) \f$ in form of similarity transform
	 @param b  Return \f$ s \times sin(theta) \f$ in form of similarity transform
	 @param tx  Return \f$ Tx \f$ in form of similarity transform
	 @param ty  Return \f$ Ty \f$ in form of similarity transform
	*/
	void CalcGlobalShape(asm_shape& shape, const CvMat* p, 
							double a, double b, double tx, double ty);

	/**
     Pyramid fitting at one certain level.
	 @param shape the point feature data
	 @param image the image resource
	 @param ilev one certain pyramid level
	 @param iter_no the number of iteration
	*/
	void PyramidFit(asm_shape& shape, const IplImage* image, int ilev, int iter_no);

private:

	CvMat*  m_M;   /**< mean vector of shape data */
    CvMat*  m_B;   /**< eigenvetors of shape data */
    CvMat*  m_V;   /**< eigenvalues of shape data */

	CvMat* m_SM;   /**< mean of shapes projected space */	
	CvMat* m_SSD;  /**< standard deviation of shapes projected space	*/	


	ASM_PROFILE_TYPE m_type;	/**< the type of sampling profile */
	
	/**< the profile distribution model */
	union
	{
		struct profile_lbp_model* lbp_tdm;			/**< lbp profile model */
		struct profile_Nd_model* classical_tdm;	/**< 1d/2d profile model */
	}; 
	
	
	asm_shape m_asm_meanshape;			/**< mean shape of aligned shapes */
	
	int m_nPoints;					/**< number of shape points */
	int m_nWidth;					/**< width of each landmark's profile */
	int m_nLevels;					/**< pyramid level of multi-resolution */
	int m_nModes;					/**< number of truncated eigenvalues */
	double m_dReferenceFaceWidth;	/**< width of reference face  */
	bool m_bInterpolate;			/**< whether to using image interpolate or not*/
	double m_dMeanCost;		/**< the mean of fitting cost to determine whether fitting succeed or not*/
	double m_dVarCost;		/**< the variance of fitting cost determine whether fitting succeed or not*/

private:
	CvMat*		m_CBackproject; /**< Cached variables for speed up */
	CvMat*		m_CBs;			/**< Cached variables for speed up */
	double*		m_dist;			/**< Cached variables for speed up */
	asm_profile* m_profile;		/**< Cached variables for speed up */
	asm_shape	m_search_shape;	/**< Cached variables for speed up */
	asm_shape	m_temp_shape;	/**< Cached variables for speed up */
};

/** You can define your own face detector function here
 @param shapes Returned face detected box which stores the Top-Left and Bottom-Right points, so its \a NPoints() = 2 here.
 @param image Image resource.
 @return false on no face exists in image, true otherwise.
*/
typedef	bool (*detect_func)(asm_shape& shape, const IplImage* image);

#endif  // _ASM_LIBRARY_H_

