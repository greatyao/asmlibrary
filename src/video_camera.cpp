/*
 * ASMLibrary(Active Shape Model Library) -- A compact SDK for face alignment
 *
 * Copyright (c) 2008-2010, Yao Wei <njustyw@gmail.com>,
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
Implemention for handling camera and avi-video

Copyright (c) 2008-2010 by Yao Wei <njustyw@gmail.com>, all rights reserved.

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 5.0-2010-5-20
*/

#include "video_camera.h"
#include <stdio.h>

/************************************************************************/
/* some stuff for video file                                            */
/************************************************************************/
static CvCapture* v_capture = NULL;		/**< Used for function cvQueryFrame */
static IplImage* v_capimg = NULL;		/**< Original image captured from video */
static IplImage *v_image = NULL;		/**< Flipped Image captured from video (when nessary) */

/************************************************************************/
/* some stuff for live camara                                            */
/************************************************************************/
static CvCapture* c_capture = NULL;		/**< Used for function cvQueryFrame */
static IplImage* c_capimg = NULL;		/**< Original image captured from video */
static IplImage *c_image = NULL;		/**< Flipped Image captured from video (when nessary) */


int open_video(const char* filename)
{
	v_capture = cvCaptureFromAVI(filename);
	if(!v_capture)
	{
		fprintf(stderr, "ERROR(%s, %d): CANNOT open video file %s!\n", 
			__FILE__, __LINE__, filename);
		return -1;
	}
		
	cvSetCaptureProperty(v_capture, CV_CAP_PROP_POS_FRAMES, 0);
	v_capimg = cvQueryFrame(v_capture );
	v_image = cvCreateImage(cvGetSize(v_capimg), v_capimg->depth, v_capimg->nChannels);

	return (int)cvGetCaptureProperty(v_capture, CV_CAP_PROP_FRAME_COUNT);
}


void close_video()
{
	cvReleaseCapture(&v_capture);
	v_capture = 0;
	cvReleaseImage(&v_image);
	v_image = 0;
}

IplImage* read_from_video(int frame_no )
{
	if(frame_no < -1)
		return 0;
	
	else
	{
		cvSetCaptureProperty(v_capture, CV_CAP_PROP_POS_FRAMES, frame_no);
		v_capimg = cvQueryFrame(v_capture );
	}
	
	if(v_capimg->origin == 0)
		cvCopy(v_capimg, v_image);
	else
		cvFlip(v_capimg, v_image);

	return v_image;
}

bool open_camera(int index)
{
	c_capture = cvCaptureFromCAM(index);
	if(!c_capture)	return false;

	cvGrabFrame(c_capture );
	c_capimg = cvQueryFrame(c_capture);
	c_image = cvCreateImage(cvGetSize(c_capimg), c_capimg->depth, c_capimg->nChannels);

	return true;
}

void close_camera()
{
	cvReleaseCapture(&c_capture);
	c_capture = 0;
	cvReleaseImage(&c_image);
	c_image = 0;
}

IplImage* read_from_camera()
{
	c_capimg = cvQueryFrame(c_capture );
	
	if(c_capimg->origin == 0)
		cvCopy(c_capimg, c_image);
	else
		cvFlip(c_capimg, c_image);

	return c_image;
}