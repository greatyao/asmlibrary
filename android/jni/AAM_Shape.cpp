/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#include <string>
#include <iostream>
#include <fstream>

#include "AAM_Shape.h"

using namespace std;

AAM_Shape::AAM_Shape( const AAM_Shape &s )
{
    CopyData (s);
}


AAM_Shape& AAM_Shape::operator=(const AAM_Shape &s)
{
    CopyData (s);
    return *this;
}


AAM_Shape& AAM_Shape::operator=(float value)
{
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
    {
		m_vPoint[i].x = value;
        m_vPoint[i].y = value;
	}   
	return *this;   
}

AAM_Shape AAM_Shape::operator+(const AAM_Shape &s)const
{    
//    return AAM_Shape(*this) += s;
	
	AAM_Shape res(*this);
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        res.m_vPoint[i].x += s.m_vPoint[i].x;
        res.m_vPoint[i].y += s.m_vPoint[i].y;
    }
    return res;
	
}


AAM_Shape& AAM_Shape::operator+=(const AAM_Shape &s)
{
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        m_vPoint[i].x += s.m_vPoint[i].x;
        m_vPoint[i].y += s.m_vPoint[i].y;
    }
    return *this;
}


AAM_Shape AAM_Shape::operator-(const AAM_Shape &s)const
{    
//	return AAM_Shape(*this) -= s;
	
    AAM_Shape res(*this);
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        res.m_vPoint[i].x -= s.m_vPoint[i].x;
        res.m_vPoint[i].y -= s.m_vPoint[i].y;
    }
    return res;
}


AAM_Shape& AAM_Shape::operator-=(const AAM_Shape &s)
{
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        m_vPoint[i].x -= s.m_vPoint[i].x;
        m_vPoint[i].y -= s.m_vPoint[i].y;
    }
    return *this;
}


AAM_Shape AAM_Shape::operator*(float value)const
{
//	return AAM_Shape(*this) *= value;
	
    AAM_Shape res(*this);
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        res.m_vPoint[i].x *= value;
        res.m_vPoint[i].y *= value;
    }
    return res;
}


AAM_Shape& AAM_Shape::operator*=(float value)
{
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        m_vPoint[i].x *= value;
        m_vPoint[i].y *= value;
    }
    return *this;
}


float AAM_Shape::operator*(const AAM_Shape &s)const
{
    float result = 0.0;
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        result += m_vPoint[i].x * s.m_vPoint[i].x +
			m_vPoint[i].y * s.m_vPoint[i].y;
    }
    return result;
}


AAM_Shape AAM_Shape::operator/(float value)const
{
 //   return AAM_Shape(*this) /= value;

	assert (value != 0);

    AAM_Shape res(*this);
    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        res.m_vPoint[i].x /= value;
        res.m_vPoint[i].y /= value;
    }
    return res;
}


AAM_Shape& AAM_Shape::operator/=(float value)
{
    assert (value != 0);

    for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        m_vPoint[i].x /= value;
        m_vPoint[i].y /= value;
    }
    return *this;
}

bool AAM_Shape::operator==(float value)
{
	for (int i = 0, size = m_vPoint.size(); i < size; i++)
	{
        if( fabs(m_vPoint[i].x - value) > 1e-6)
			return false;

		if( fabs(m_vPoint[i].y - value) > 1e-6)
			return false;
    }

	return true;
}
	

void AAM_Shape::CopyData(const AAM_Shape &s )
{
	if(m_vPoint.size() != s.m_vPoint.size())
		m_vPoint.resize(s.m_vPoint.size());
	m_vPoint = s.m_vPoint;
}



//----------------------------------------------------------------------
// Get Euclidean Norm of Shape
//----------------------------------------------------------------------
float AAM_Shape::GetNorm2()const
{
    float norm = 0.0;

    // Normalize the vector to unit length, using the 2-norm.
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        norm += pow(m_vPoint[i].x, 2);
        norm += pow(m_vPoint[i].y, 2);
    }
    norm = sqrt(norm);
    return norm;
}

//---------------------------------------------------------------------
// Get the center of Shape
//----------------------------------------------------------------------
void AAM_Shape::COG(float &x, float &y)const
{
    x = y = 0.0;

    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        x += m_vPoint[i].x;
        y += m_vPoint[i].y;
    }
    x /= m_vPoint.size();
    y /= m_vPoint.size();

}

//---------------------------------------------------------------------
// Translate Shape so that its center of gravity is at the origin
//----------------------------------------------------------------------
void AAM_Shape::Centralize()
{
    float xSum, ySum;

	COG(xSum, ySum);

	Translate(-xSum, -ySum);

}

//---------------------------------------------------------------------
// Translate Shape using (x, y) 
//----------------------------------------------------------------------
void AAM_Shape::Translate(float x, float y)
{
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        m_vPoint[i].x += x;
        m_vPoint[i].y += y;
    }
}

//---------------------------------------------------------------------
// Scale Shape using s
//----------------------------------------------------------------------
void AAM_Shape::Scale(float s)
{
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        m_vPoint[i].x *= s;
        m_vPoint[i].y *= s;
    }
}

//----------------------------------------------------------------------
// Scale horizontal and vertical respectively
//----------------------------------------------------------------------
void AAM_Shape::ScaleXY(float sx, float sy)
{
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        m_vPoint[i].x *= sx;
        m_vPoint[i].y *= sy;
    }
}

float AAM_Shape::Normalize()
{
    float x,y;

    COG(x, y);

    Translate( -x, -y );

    float norm = GetNorm2();
	norm = (norm < 1e-10) ? 1 : norm; 

    Scale( 1./norm );

    return norm;
}

//----------------------------------------------------------------------
// Shape = (c00, c01; c10, c11) * Shape
//----------------------------------------------------------------------
void AAM_Shape::Transform(float c00, float c01, float c10, float c11)
{
	float x, y;
	for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        x = m_vPoint[i].x;
        y = m_vPoint[i].y;
        m_vPoint[i].x = c00*x+c01*y;
        m_vPoint[i].y = c10*x+c11*y;
    }

}

//----------------------------------------------------------------------
// Rotate Shape as theta
//----------------------------------------------------------------------
void AAM_Shape::Rotate(float theta)
{
    // set up transform matrix of rotation
    float c00 =  cos( theta );
    float c01 = -sin( theta );
    float c10 =  sin( theta );
    float c11 =  cos( theta );

	Transform(c00, c01, c10, c11);   
}

// ---------------------------------------------------------------------------
// Align Shape to to RefShape using a similarity transform.
// See also algorithm C.3 in Appendix C of [Cootes & Taylor, 2004]
// www.isbe.man.ac.uk/~bim/Models/app_models.pdf.
// returns the similarity transform matrix:
//      a -b  Tx
//      b  a  Ty
//      0  0  1
// Note BY YAO Wei: the Shape itself doesn't change
// ---------------------------------------------------------------------------
void AAM_Shape::AlignTransformation(const AAM_Shape &ref, 
		float &a, float &b, float &tx, float &ty)const
{

	float X1 = 0, Y1 = 0, X2 = 0, Y2 = 0, Z = 0, C1 = 0, C2 = 0;
	float W = m_vPoint.size();
	float x1, y1, x2, y2;
	
	for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
		x1 = ref.m_vPoint[i].x;
        y1 = ref.m_vPoint[i].y;
		x2 = m_vPoint[i].x;
        y2 = m_vPoint[i].y;
		
		Z  += x2 * x2 + y2 * y2;
		X1 += x1;
		Y1 += y1;
		X2 += x2;
		Y2 += y2;
		C1 += x1 * x2 + y1 * y2;
		C2 += y1 * x2 - x1 * y2;
	}
	
	{
		float SolnA[] = {X2, -Y2, W, 0, Y2, X2, 0, W, Z, 0, X2, Y2, 0, Z, -Y2, X2};
		CvMat A = cvMat(4, 4, CV_32FC1, SolnA);
		float SolnB[] = {X1, Y1, C1, C2};
		CvMat B = cvMat(4, 1, CV_32FC1, SolnB);

		static CvMat* Soln = cvCreateMat(4, 1, CV_32FC1);
		cvSolve(&A, &B, Soln, CV_SVD);

		a	= CV_MAT_ELEM(*Soln, float, 0, 0);  b	= CV_MAT_ELEM(*Soln, float, 1, 0);
		tx	= CV_MAT_ELEM(*Soln, float, 2, 0);	 ty	= CV_MAT_ELEM(*Soln, float, 3, 0);
	}

	// Explained by YAO Wei, 2008.01.29.
	// It is equivalent as follows, but the former method of is more robust.
	/************************************************************************/
	/*		a	=	C1 / Z			b	=	C2 / Z							*/
	/*		tx	=	X1 / W			ty	=	Y1 / W							*/
	/************************************************************************/	
}

//----------------------------------------------------------------------
// The same as above, but the Shape itself will change
//----------------------------------------------------------------------
void AAM_Shape::AlignTo(const AAM_Shape &ref)
{
	float a, b, tx, ty;

	AlignTransformation(ref, a, b, tx, ty);
	
	TransformPose(a, b, tx, ty);
}

//----------------------------------------------------------------------
// Shape = (a, -b; b, a) * Shape + (tx, ty)
//----------------------------------------------------------------------
void AAM_Shape::TransformPose(float a, float b, float tx, float ty)
{
    Transform(a, -b, b, a);
	
	Translate( tx, ty );
}

const float AAM_Shape::MinX()const
{	
    float val, min = 3.4E+38;

    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {	
        val = m_vPoint[i].x;
        min = val<min ? val : min;
    }
    return min;
}

const float AAM_Shape::MinY()const
{	
    float val, min = 3.4E+38;
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {	
        val = m_vPoint[i].y;
        min = val<min ? val : min;
    }
    return min;
}


const float AAM_Shape::MaxX()const
{	
    float val, max = -3.4E+38;
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        val = m_vPoint[i].x;
        max = val>max ? val : max;
    }
    return max;
}


const float AAM_Shape::MaxY()const
{	
    float val, max = -3.4E+38;
    for(int i = 0, size = m_vPoint.size(); i < size; i++)
    {
        val = m_vPoint[i].y;
        max = val>max ? val : max;
    }
    return max;
}
	
void AAM_Shape::Mat2Point(const CvMat* res)
{
    int nPoints = res->cols / 2;
	float *B = (float*)(res->data.ptr+res->step*0);
	if(m_vPoint.size() != nPoints)		resize(nPoints);
	for (int i = 0; i < nPoints ; i++)
    {
        m_vPoint[i].x = B[2*i];
        m_vPoint[i].y = B[2*i+1];
	}
}

void AAM_Shape::Point2Mat(CvMat* res)const
{
	int nPoints = res->cols / 2;
	float *B = (float*)(res->data.ptr+res->step*0);
	for (int i = 0; i < nPoints ; i++)
    {
        B[2*i] = m_vPoint[i].x;
        B[2*i+1] = m_vPoint[i].y;
	}
}

bool AAM_Shape::ReadAnnotations(const std::string &filename)
{
	bool isAbsolute;
	if(strstr(filename.c_str(), ".asf")|| strstr(filename.c_str(), ".ASF"))
	{
		isAbsolute = false;
		ReadASF(filename);
	}
	else if(strstr(filename.c_str(), ".pts") || strstr(filename.c_str(), ".PTS"))
	{
		isAbsolute = true;
		ReadPTS(filename);
	}
	return isAbsolute;
}

void AAM_Shape::ReadASF(const std::string &filename)
{
    fstream fp;
    fp.open(filename.c_str(), ios::in);

    string temp;

    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);

    int NbOfPoints = atoi(temp.c_str());

    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);

    CvPoint2D32f temppoint;
	m_vPoint.clear();
    for (int i = 0; i < NbOfPoints; i++)
    {
        getline(fp, temp, ' ');
        getline(fp, temp, ' ');
		getline(fp, temp, ' ');
        // In DTU IMM , x means rows from left to right
        temppoint.x = atof(temp.c_str());
        getline(fp, temp, ' ');
        // In DTU IMM , y means cols from top to bottom
        temppoint.y = atof(temp.c_str());
        getline(fp, temp, ' ');
        getline(fp, temp, ' ');
        getline(fp, temp);
        // In sum, topleft is (0,0), right bottom is (640,480)
        m_vPoint.push_back (temppoint);
   }
	
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    fp.close ();
}

void AAM_Shape::ReadPTS(const std::string &filename)
{
	fstream fp;
    fp.open(filename.c_str(), ios::in);

    string temp;

    getline(fp, temp);

	getline(fp, temp, ' ');
	getline(fp, temp);
	int NbOfPoints = atoi(temp.c_str());

	getline(fp, temp);
    
	CvPoint2D32f temppoint;
	m_vPoint.clear();
    for (int i = 0; i < NbOfPoints; i++)
    {
        fp >> temppoint.x >> temppoint.y;
		m_vPoint.push_back (temppoint);
    }

    getline(fp, temp);

    fp.close ();
}

void AAM_Shape::Read(std::ifstream& is)
{
	for (int i = 0, nPoints = NPoints(); i < nPoints ; i++)
		is.read((char*)&m_vPoint[i], sizeof(CvPoint2D32f));
}

void AAM_Shape::Write(std::ofstream& os)
{
	for (int i = 0, nPoints = NPoints(); i < nPoints ; i++)
		os.write((char*)&m_vPoint[i], sizeof(CvPoint2D32f));
}
