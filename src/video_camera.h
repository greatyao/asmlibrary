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
Routines for handling camera and avi-video

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2011-5-28
*/

#ifndef _VIDEO_CAMERA_H_
#define _VIDEO_CAMERA_H_

#include "asmlibrary.h"
#include <cv.h>
#include <highgui.h>

/**
 Open an AVI file.
 @param filename the video file located in
 @return -1 on failure, frame count of the video otherwise
*/
int open_video(const char* filename);

/**
 Get one certain frame of video.
 @param frame_no  which frame
 @return Internal IplImage ptr
*/
IplImage* read_from_video(int frame_no);

/**
 Close avi and release memory.
 */
void close_video();


/**
 Capture from live camera.
 @param index camara index
 @return false on failure, true otherwise
 */
bool open_camera(int index);

/**
 Get one certain frame of live camera.
 @return Internal IplImage ptr
*/
IplImage* read_from_camera();

/**
 Close camara and release memory.
 */
void close_camera();


#endif  // _VIDEO_CAMERA_H_

