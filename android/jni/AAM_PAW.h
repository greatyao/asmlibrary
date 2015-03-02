/****************************************************************************
*						AAMLibrary
*			http://code.google.com/p/aam-library
* Copyright (c) 2008-2009 by GreatYao, all rights reserved.
****************************************************************************/

#ifndef AAM_PAW_H
#define AAM_PAW_H

#include "AAM_Util.h"
#include "AAM_Shape.h"

class AAM_IC;
class AAM_Basic;
class AAM_CAM;

// 2D piecewise affine warp
class AAM_PAW  
{
	friend class AAM_IC;
	friend class AAM_Basic;
	friend class AAM_CAM;
	friend class AAM_Common;
public:
	AAM_PAW();
	~AAM_PAW();

	// Build a piecewise affine warp
	void Train(const AAM_Shape& ReferenceShape, 
		CvMat* Points,
		CvMemStorage* Storage,
		const std::vector<std::vector<int> >* tri = 0,
		bool buildVtri = true);

	// Read data from stream 
	void Read(std::ifstream& is);

	// Write data to stream
	void Write(std::ofstream& os);

	inline const int nPoints()const { return __n;	}
	inline const int nPix()const	{ return __nPixels;	}
	inline const int nTri()const	{ return __nTriangles;	}

	inline const CvPoint2D32f Vertex(int i)const { return __referenceshape[i];	}
	
	inline const std::vector<std::vector<int> >* GetTri()const{  return &__tri;	}
	
	// index of point for j-th vertex of i-th triangle 
	inline const int Tri(int i, int j)const{ return __tri[i][j];	}

	// index of triangle the pixel lies in
	inline const int PixTri(int i)const{ return __pixTri[i];	}

	// Coeffients of affine warp
	inline const float Alpha(int i)const { return __alpha[i];	}
	inline const float Belta(int i)const { return __belta[i];	}
	inline const float Gamma(int i)const { return __gamma[i];	}

	// width and height boundary  
	inline const int Width()const { return __width;	}
	inline const int Height()const { return __height;	}
	// Is point(j,i) in boundary: not(-1), yes(index of pixel)
	inline const int Rect(int i, int j)const { return __rect[i][j]; } 

	// Warp the image to the mean shape mesh to calculate shape free patch
	void CalcWarpTexture(const CvMat* s, const IplImage* image,	CvMat* t)const;

	// For display, translate the texture vector to the visual-able image format
	// and then save into an image
	void TextureToImage(IplImage* image, const CvMat* t)const;
	void SaveWarpTextureToImage(const char* filename, const CvMat* t)const;
	
	// Calculate the warp parameters for a specific point according the piecewise
	static void CalcWarpParameters(float x, float y, float x1, float y1,
								float x2, float y2, float x3, float y3, 
								float &alpha, float &belta, float &gamma);

	// Warp from source triangle to destination triangle
	static void Warp(float x, float y, float x1, float y1,
		float x2, float y2, float x3, float y3,
		float& X, float& Y, float X1, float Y1,
		float X2, float Y2, float X3, float Y3);

private:
	// Build triangles
	void Delaunay(const CvSubdiv2D* Subdiv, const CvMat *ConvexHull);

	// Calculate all pixels in the triangles of mean shape mesh
	void CalcPixelPoint(const CvRect rect, CvMat *ConvexHull);
	void FastCalcPixelPoint(const CvRect rect);

	// Is the current edge (ind1, ind2) already in the AAM model edges?
	static bool IsEdgeIn(int ind1, int ind2,
						const std::vector<std::vector<int> > &edges);

	// Help to build up triangles in the mesh
	static bool IsTriangleNotIn(const std::vector<int>& one_tri,
							const std::vector<std::vector<int> > &tris);

	// Find triangles containing each landmark.
	void FindVTri();

	int FastFillConvexPoly(CvPoint2D32f pts[3], void* data);

private:
	int __n;						/*number of landmarks*/
	int __nPixels;					/*number of pixels*/
	int __nTriangles;				/*number of triangles*/
	int __width, __height, __xmin, __ymin; /*Domain of warp region*/
	
	std::vector<std::vector<int> > __tri;	/*triangle vertexes index*/
	std::vector<std::vector<int> > __vtri;	/*vertex vs triangle*/
	std::vector<int>			   __pixTri;
	std::vector<float> __alpha, __belta,  __gamma; 
	std::vector<std::vector<int> > __rect; /*height by width */

	AAM_Shape __referenceshape; 



};

#endif // 
