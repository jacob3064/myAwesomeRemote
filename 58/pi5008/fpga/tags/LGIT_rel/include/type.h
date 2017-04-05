/**
 * \file
 *
 * \brief	type
 *
 * \author 	yhcho
 *
 * \version 0.1
 *
 * Revision History
 *
 *
 * Copyright (c) 2016 Pixelplus Co.,Ltd. All rights reserved
 *
 */

#ifndef __TYPE_H__
#define __TYPE_H__

typedef char						int8;
typedef unsigned char				uint8;
typedef short						int16;
typedef unsigned short				uint16;
typedef int							int32;
typedef unsigned int				uint32;

typedef volatile unsigned int		vuint32;

typedef enum {FALSE, TRUE}			bool;

//#define NULL						0

#define APICALL						extern
#define APIENTRY

#define IN
#define OUT

#endif // __TYPE_H__