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
Routines for Viola and Jones's AdaBoost Haar-like Face Detector 

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#ifndef _VJFACE_DETECT_H_
#define _VJFACE_DETECT_H_

#include "asmlibrary.h"


/**
 Load adaboost cascade file for detect face.
 @param cascade_name Filename the cascade detector located in
 @return false on failure, true otherwise
*/
bool init_detect_cascade(const char* cascade_name = "haarcascade_frontalface_alt2.xml");


/**
 Release the memory of adaboost cascade face detector
*/
void destory_detect_cascade();

/**
 Detect only one face from image, and this human face is located as close as to the center of image
 @param shape return face detected box which stores the Top-Left and Bottom-Right points, so its \a NPoints() = 2 here
 @param image the image resource
 @return false on no face exists in image, true otherwise
*/
bool detect_one_face(asm_shape& shape, const IplImage* image);


/**
 Detect all human face from image.
 @param shapes return face detected box which stores the Top-Left and Bottom-Right points, so its \a NPoints() = 2 here
 @param n_shapes the numbers of faces to return
 @param image the image resource
 @return false on no face exists in image, true otherwise
*/
bool detect_all_faces(asm_shape** shapes, int& n_shapes, const IplImage* image);

/**
 Release the shape resource allocated by detect_all_faces().
 @param shapes the ptr of asm_shape [] 
*/
void free_shape_memeory(asm_shape** shapes);



#endif  // _VJFACE_DETECT_H_

