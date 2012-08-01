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
Classes for implementing face alignment/tracking using active shape model.

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#ifndef _ASM_FITTING_H_
#define _ASM_FITTING_H_

#include "asmlibrary.h"

/** Wrapped Class for face alignment/tracking using active shape model */
class ASMLIB asmfitting
{
public:
	/** Constructor */
	asmfitting();

	/** Destructor */
	~asmfitting();
	
	/**
     Process face alignment on image. (Only for one face box)
	 @param shape the point features that carries initial shape and also restores result after fitting
	 @param image the image resource
	 @param n_iteration the number of iteration during fitting
	*/
	void Fitting(asm_shape& shape, const IplImage* image, int n_iteration = 30);
	
	/**
     Process face alignment on image. (For multi-face boxes)
	 @param shapes all shape datas that carry the fitting result
	 @param n_shapes the number of human face
	 @param image the image resource
	 @param n_iteration the number of iteration during fitting
	*/
	void Fitting2(asm_shape* shapes, int n_shapes, const IplImage* image, int n_iteration = 30);
	
	/**
     Process face tracking on video/camera.
	 @param shape the point features that carries initial shape and also restores result after fitting
	 @param image the image resource
	 @param frame_no one certain frame number of video/camera
	 @param bopticalflow whether to use optical flow or not?
	 @param n_iteration the number of iteration during fitting
	 @return false on failure, true otherwise.
	 */
	bool ASMSeqSearch(asm_shape& shape, const IplImage* image, 
		int frame_no = 0, bool bopticalflow = false, int n_iteration = 30);
	
	/**<
     Get the Average Viola-Jone Box.
	*/
	const asm_shape GetMappingDetShape()const { return m__VJdetavshape;}
	
	/**<
     Get the width of mean face.
	*/
	const double	GetMeanFaceWidth()const{ return m_model.GetMeanShape().GetWidth();	}
	
	/**<
	 Get raw ptr of asm_model.
	*/
	const asm_model* GetModel()const { return &m_model; }
	
	/**
     Read model data from file.
	 @param filename the filename that stores the model
	 @return false on failure, true otherwise
    */
	bool Read(const char* filename);

	/**
     Draw point and edge on the image.
	 @param image  the image resource
	 @param shape  the shape after fitting
	*/
	void Draw(IplImage* image, const asm_shape &shape);

private:

	/**
     Apply optical flow between two successive frames.
	 @param shape it carries initial shape and also restores result after fitting
	 @param grayimage the image resource.
	*/
	void OpticalFlowAlign(asm_shape& shape, const IplImage* grayimage);

private:
	asm_model	m_model;	/**<active shape model to be trained */
	int *m_edge_start; /**< Starting index of edges */
	int *m_edge_end;   /**< Ending index of edges */
	int m_nEdge;       /**< Number of edges */
	asm_shape m__VJdetavshape;    /**< average mapping shape relative to VJ detect box*/
	scale_param m_param;			/**< point index of left and right side in the face template*/
	bool m_flag;					/**< Does the image contain face? */
	double m_dReferenceFaceWidth;	/**< reference face width */

private:
	IplImage* __lastframe;  /**< Cached variables for optical flow */
	IplImage* __pyrimg1;	/**< Cached variables for optical flow */
	IplImage* __pyrimg2;	/**< Cached variables for optical flow */
	Point2D32f* __features1;	/**< Cached variables for optical flow */
	Point2D32f* __features2;	/**< Cached variables for optical flow */
	char* __found_feature;	/**< Cached variables for optical flow */
	float* __feature_error;	/**< Cached variables for optical flow */
};

#endif //_ASM_FITTING_H_


