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
Implemention for Viola and Jones's AdaBoost Haar-like Face Detector 

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#include "vjfacedetect.h"
#include <cv.h>
#include <highgui.h>

static CvMemStorage* __storage = NULL; 		/**< Memory storage */
static CvHaarClassifierCascade* __cascade = NULL;	/**< Haar cascade classifier */


bool init_detect_cascade(const char* cascade_name)
{
	__cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	if(__cascade == 0)
	{
		printf("ERROR(%s, %d): Can't load cascade file!\n", __FILE__, __LINE__);
		return false;
	}	
	__storage = cvCreateMemStorage(0);
	return true;
}


void destory_detect_cascade()
{
	cvReleaseMemStorage(&__storage);
	cvReleaseHaarClassifierCascade(&__cascade);
}

bool detect_all_faces(asm_shape** Shapes, int& n_shapes, const IplImage* image)
{
	IplImage* pWork = cvCreateImage
		(cvSize(image->width/2, image->height/2), image->depth, image->nChannels);
	cvPyrDown(image, pWork, CV_GAUSSIAN_5x5 );
		
	CvSeq* pFaces = cvHaarDetectObjects(pWork, __cascade, __storage,
		1.1, 3, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30));

	cvReleaseImage(&pWork);
	
	n_shapes = 0;
	if(0 == pFaces->total)//can't find a face
		return false;

	*Shapes = new asm_shape[pFaces->total];
	n_shapes = pFaces->total;
	for (int iFace = 0; iFace < pFaces->total; iFace++)
    {
		(*Shapes)[iFace].Resize(2);
		CvRect* r = (CvRect*)cvGetSeqElem(pFaces, iFace);
		
		CvPoint pt1, pt2;
		pt1.x = r->x * 2;
		pt2.x = (r->x + r->width) * 2;
		pt1.y = r->y * 2;
		pt2.y = (r->y + r->height) * 2;
	
		(*Shapes)[iFace][0].x  = r->x*2.0;
		(*Shapes)[iFace][0].y  = r->y*2.0;
		(*Shapes)[iFace][1].x  = (*Shapes)[iFace][0].x + 2.0*r->width;
		(*Shapes)[iFace][1].y  = (*Shapes)[iFace][0].y + 2.0*r->height;
    }
	return true;
}

void free_shape_memeory(asm_shape** shapes)
{
	delete [](*shapes);
}


bool detect_one_face(asm_shape& Shape, const IplImage* image)
{
	asm_shape* detShape;
	int nfaces;

	bool flag = detect_all_faces(&detShape, nfaces, image);

	if(!flag) return false;

	// get most central face
	int iSelectedFace = 0;
	double x0 = image->width/2., y0 = image->height/2.;
	double x, y, d, D = 1e307;
	// max abs dist from center of face to center of image
	for (int i = 0; i < nfaces; i++)
    {
		x = (detShape[i][0].x + detShape[i][1].x) / 2.;
		y = (detShape[i][0].y + detShape[i][1].y) / 2.;
		d = sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0)); 
		if (d < D)
        {
			D = d;
			iSelectedFace = i;
        }
    }
	Shape = detShape[iSelectedFace];
	delete []detShape;

	return true;
}
