/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#ifndef AAM_SHAPE_H
#define AAM_SHAPE_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2//highgui/highgui_c.h>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>

class AAM_Shape
{
private:
	// point data.
	std::vector<CvPoint2D32f> m_vPoint; 

public:
    // Constructors and Destructor
    AAM_Shape(){ resize(0); }
    AAM_Shape(const AAM_Shape &v);
    ~AAM_Shape() { clear(); }

	//access elements
	const CvPoint2D32f operator[] (int i)const{  return m_vPoint[i];	}
	CvPoint2D32f& operator[] (int i){  return m_vPoint[i];	}
	
	inline const int NPoints()const{ return	m_vPoint.size(); }

    // operators
    AAM_Shape&    operator=(const AAM_Shape &s);
    AAM_Shape&    operator=(float value);
    AAM_Shape     operator+(const AAM_Shape &s)const;
    AAM_Shape&    operator+=(const AAM_Shape &s);
    AAM_Shape     operator-(const AAM_Shape &s)const;
    AAM_Shape&    operator-=(const AAM_Shape &s);
    AAM_Shape     operator*(float value)const;
    AAM_Shape&    operator*=(float value);
    float        operator*(const AAM_Shape &s)const;
    AAM_Shape     operator/(float value)const;
    AAM_Shape&    operator/=(float value);
	bool		  operator==(float value);

    void    clear(){ resize(0); }
    void    resize(int length){ m_vPoint.resize(length); }
	
	void	Read(std::ifstream& is);
	void	Write(std::ofstream& os);
	bool	ReadAnnotations(const std::string &filename);
	void    ReadASF(const std::string &filename);
	void	ReadPTS(const std::string &filename);

	const float  MinX()const;
    const float  MinY()const;
    const float  MaxX()const;
    const float  MaxY()const;
    inline const float  GetWidth()const{  return MaxX()-MinX();    }
	inline const float  GetHeight()const{ return MaxY()-MinY();	}
	
    // Transformations
    void    COG(float &x, float &y)const;
    void    Centralize();
    void    Translate(float x, float y);
    void    Scale(float s);
    void    Rotate(float theta);
	void    ScaleXY(float sx, float sy);
	float	Normalize();
	
	// Align the shapes to reference shape 
	//													[a -b Tx]
	// returns the similarity transform: T(a,b,tx,ty) = [b  a Ty]
	//													[0  0  1]
	void    AlignTransformation(const AAM_Shape &ref, 
			float &a, float &b, float &tx, float &ty)const;
    
	// Align the shapes to reference shape as above, but no returns
    void    AlignTo(const AAM_Shape &ref);
    
	// Transform Shape using similarity transform T(a,b,tx,ty)
	void    TransformPose(float a, float b, float tx, float ty);

	// Euclidean norm
	float  GetNorm2()const;

	// conversion between CvMat and AAM_Shape
	void    Mat2Point(const CvMat* res);
	void    Point2Mat(CvMat* res)const;

private:
	void    CopyData(const AAM_Shape &s);
	void    Transform(float c00, float c01, float c10, float c11);

};


#endif  // asmutil_h

