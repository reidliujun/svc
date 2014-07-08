//
//	aWarpSharp (version 1.0) - WarpSharping filter
//
//	aWarpSharp ffdshow api.
//
//	Copyright (C) 2003 Marc Fauconneau
//
//	designed for ffdshow only. thx.

#ifndef __AWS_API_H__
#define __AWS_API_H__

/* basic YV12 plane struct */
struct awsapi_plane {
	unsigned char* ptr;
	int height;
	int width;
	int pitch;
};

/* basic YV12 frame struct */
struct awsapi_frame {
	awsapi_plane yplane,uplane,vplane;
};

/*	all the stuff it needs.
	all/f
*/
struct awsapi_settings {
	awsapi_frame src,dst,work;
	int thresh,blurlevel,depth;
	int cm,bm;
	bool show;
};

void aws_run(awsapi_settings *set);

#endif /* __AWS_API_H__ */
