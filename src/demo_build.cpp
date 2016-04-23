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
A demo show how to build a active shape model 

Please cite the following or equivalent reference in any publicly available 
text that uses asmlibrary: 

YAO Wei. Research on Facial Expression Recognition and Synthesis.
<EM> Master Thesis, Department of Computer Science and Technology, 
Nanjing University</EM>, Feb 2009. \url http://code.google.com/p/asmlibrary

@version 6.0-2010-5-30
*/

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>

#include "asmbuilding.h"
#include "vjfacedetect.h"

using namespace std;
typedef vector<string> filelists;

#ifdef WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

static int match(const char* s1, const char* s2)
{
	int diff = strlen(s1) - strlen(s2);
	if(diff >= 0 && !strcasecmp(s1+diff, s2))
		return 1;
	return 0;
}

#endif

static void print_version()
{
	printf("\n\n"
		   "/**************************************************************/\n"
		   "/*      ASMLibrary -- A compact SDK for face alignment        */\n"
		   "/*  Copyright (c) 2008-2011 by Yao Wei, all rights reserved.  */\n"
		   "/*                    Contact: njustyw@gmail.com              */\n"
		   "/**************************************************************/\n"
		   "\n\n");
}

static void usage_build()
{
	printf("Usage: build [options] train_path image_ext point_ext cascade_name model_file\n"
		"options:\n"
		"   -i	interpolate or not (default 0)\n"
		"   -l	profile length of half side (default 8)\n"
		"   -p	level of parymid (default 3)\n"
		"   -t	percentage of shape PCA (default 0.975)\n"
		"   -T	type of sampling profile (default PROFILE_1D)\n\n\n");
		exit(0);
}

// compare function for the qsort() call below
static int str_compare(const void *arg1, const void *arg2)
{
    return strcmp((*(string*)arg1).c_str(), (*(string*)arg2).c_str());
}

// Scan all files with the "extension" under "path" directory and sort them
static filelists ScanNSortDirectory(const char*path, const char*extension)
{
#ifdef WIN32
    WIN32_FIND_DATA wfd;
    HANDLE handle;
    string search_path, search_file;
#else
	struct dirent *d;
	DIR* dir;
	struct stat s;
	char fullpath[1024];
#endif
    filelists allfiles;
	int n = 0;

#ifdef  WIN32   
	search_path = string(path) + "/*" + string(extension);
	handle = FindFirstFile(search_path.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == handle)
    {
		fprintf(stderr, "ERROR(%s, %d): Cannot find (*.%s)files in directory %s\n",
			__FILE__, __LINE__, extension, path);
		exit(0);
    }
    do
    {
        if (wfd.cFileName[0] == '.')
        {
            continue;
        }
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
	    }
        else
        {
			search_file = string(path) + "/" + string(wfd.cFileName);
			allfiles.push_back(search_file);
			n++;
		}
    }while (FindNextFile(handle, &wfd));

    FindClose(handle);
#else
	
	dir = opendir(path);
	if(dir == NULL)	
	{
		fprintf(stderr, "ERROR(%s, %d): Can not open directory %s\n", 
			__FILE__, __LINE__, path);
		exit(0);
	}

	while(d = readdir(dir))
	{
		sprintf(fullpath, "%s/%s", path, d->d_name);
		if(stat(fullpath, &s) != -1)
		{
			if(S_ISDIR(s.st_mode))
				continue;
			if(match(d->d_name, extension))
			{
				allfiles.push_back(string(fullpath));
				n++;
			}
		}

	}
	closedir(dir);
#endif

	// sort the filenames
    qsort((void *)&(allfiles[0]), (size_t)n, sizeof(string), str_compare);

    return allfiles;
}

int main(int argc, char *argv[])
{
	int interpolate = 0;
	int width = 8;
	int level = 3;
	double percentage = 0.975;
	ASM_PROFILE_TYPE type = PROFILE_1D;
	char path[256], img_ext[20], pts_ext[20];
	char model[256], cascade_name[256];
	filelists imagelist, ptslist;
	char** imgFiles = NULL;
	char** ptsFiles = NULL;    
	int n_images, n_shapes;
	int i;

	print_version();

	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] != '-')break;
		if(++i>=argc)
			usage_build();
		switch(argv[i-1][1])
		{
		case 'i':
			interpolate = atoi(argv[i]);
			break;
		case 'l':
			width = atoi(argv[i]);
			break;
		case 'p':
			level = atoi(argv[i]);
			break;
		case 't':
			percentage = atof(argv[i]);
			break;
		case 'T':
			type = (ASM_PROFILE_TYPE)atoi(argv[i]);
			break;
		default:
			fprintf(stderr, "unknown options\n");
			usage_build();
		}
	}

	if(i+5 != argc)	usage_build();

	strcpy(path, argv[i]);
	strcpy(img_ext, argv[i+1]);
	strcpy(pts_ext, argv[i+2]);
	strcpy(cascade_name, argv[i+3]);
	strcpy(model, argv[i+4]);

	/************************************************************************/
	/* Step 0: init face detector                                           */
	/************************************************************************/
	if(init_detect_cascade(cascade_name) == false)
		return -1;
	
	/************************************************************************/
	/* Step 1: Load image and shape data                                    */
	/************************************************************************/
	imagelist = ScanNSortDirectory(path, img_ext);
	ptslist = ScanNSortDirectory(path, pts_ext);
	n_images = imagelist.size();
	n_shapes = ptslist.size();
	imgFiles = new char*[n_images];
	ptsFiles = new char*[n_shapes];
	for(i = 0; i < n_images; i++)
		imgFiles[i] = (char*)imagelist[i].c_str();
	for(i = 0; i < n_shapes; i++)
		ptsFiles[i] = (char*)ptslist[i].c_str();

	asmbuilding buildASM;
	
	/************************************************************************/
	/* Step 2: Build an ASMModel                                            */
	/************************************************************************/
	if(buildASM.Train((const char**)imgFiles, n_images, (const char**)ptsFiles, n_shapes,
		interpolate, width, percentage, level, type) == false)
		return -1;

	/************************************************************************/
	/* Step 3: Generate mapping between face-box and shape-groundtruth      */
	/************************************************************************/
	buildASM.BuildDetectMapping((const char**)imgFiles, n_images,
		(const char**)ptsFiles, n_shapes, detect_one_face);

	/************************************************************************/
	/* Step 4: Write model to file                                          */
	/************************************************************************/
	if(buildASM.Write(model) == false)
		return -1;

	destory_detect_cascade();
	
	return 0;
}



