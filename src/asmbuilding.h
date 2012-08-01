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
Classes for implementing building active shape model for face alignment/tracking.

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#ifndef _ASM_BUILDING_H_
#define _ASM_BUILDING_H_

#include "asmlibrary.h"

/** Wrapped Class for building of active shape face model */
class ASMLIB asmbuilding
{
public:
	
    /** Constructor */
	asmbuilding(); 
	
	/** Destructor */
	~asmbuilding();
	
	/**
	 Build active shape model for human face.
	 @param imagelists the lists of image files
	 @param n_images the number of image files
	 @param shapelists the lists of shape point files
	 @param n_shapes the number of shape data
	 @param binterpolate will sample pixel by bilinear interpolate or not?
	 @param halfwidth  the halfside width of profile
	 @param percentage the fraction of shape variation to retain during PCA
	 @param level_no the number of pyramid level
	 @param type the type of sampling profile
	 @return false on failure, true otherwise
	*/
	bool  Train(const char** imagelists, int n_images,
				const char** shapelists, int n_shapes,
				bool binterpolate = true, int halfwidth = 8, 
				double percentage = 0.975, int level_no = 4,
				ASM_PROFILE_TYPE type = PROFILE_1D);

	/**
	 Generate map relation between the face box and shape data groundtruth.
	 @param imagelists the lists of image files
	 @param n_images the number of image files
	 @param shapelists the lists of shape point files
	 @param n_shapes the number of shape data
	 @param my_func your implementing function for detecting only one object
	*/
	void BuildDetectMapping(const char** imagelists, int n_images, 
		const char** shapelists, int n_shapes,
		detect_func my_func);

    /**
	 Write active shape model for human face to file.
	 @param filename the filename the model writes to
	 @return false on failure, true otherwise
	*/
	bool  Write(const char* filename);

	/**<
	 Get raw ptr of asm_model.
	*/
	const asm_model* GetModel()const { return &m_model; }

private:

	/**
	 Generate meshes of the mean face.
	 @param mean_shape the mean shape of face
	*/
	void BuildMeshes(const asm_shape& mean_shape);
	
	/**
	 Generate all edges of the mean face.
	 @param mat the shape data of mean face
	*/
	void FindEdges(CvMat* mat);


private:
	
	asm_model	m_model;	/**<active shape model to be trained */	
	int *m_edge_start; /**< Starting index of edges */
	int *m_edge_end;   /**< Ending index of edges */
	int m_nEdge;       /**< Number of edges */
	asm_shape m__VJdetmeanshape;    /**< average mapping shape relative to VJ detect box*/
	int m_iLeft;    /**< Index of points in x-direction which has the minimum x in the face template */
	int m_iRight;	/**< Index of points in x-direction which has the maximum x in the face template */	
	double m_dReferenceFaceWidth;	/**< reference face width*/
};

#endif // _ASM_BUILDING_H_
