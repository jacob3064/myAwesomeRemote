/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <nds32_intrinsic.h>

#include "debug.h"
#include "system.h"
#include "du.h"
#include "sys_api.h"
#include "intr.h"

#include "svm_register.h"
#include "utils.h"
#include "svm_api.h"

#include "svm_test_LUT4FrontBack.h"
#include "svm_test_LUT4LeftRight.h"
#include "svm_test_LUT4BC.h"
#include "svm_test_carImage.h"

#undef LOG
#define LOG(x...)	{ printf("(%04d): ", __LINE__); printf(x); }


/***************************************************************************************************************************************************************
 * utils
***************************************************************************************************************************************************************/
static volatile   int delayTemp;
static void svmtest_sleep(uint32 ms)
{
	ms *= 17000;
	while(ms-- > 0)
		delayTemp = ms;
}

static void svmtest_inputPattern(bool isEnable)
{
	if ( isEnable )
		SetRegValue(0xf01000e8, 0x8c);
	else
		SetRegValue(0xf01000e8, 0x0);
}

static void svmtest_inputClear(void)
{
	SetRegValue(0xf01000e8, 0x80);		// black test pattern
	svmtest_sleep(3000);
	SetRegValue(0xf01000e8, 0x0);
}

/***************************************************************************************************************************************************************
 * set default value of register
***************************************************************************************************************************************************************/
static void svmtest_setRegInit(void)
{
	int i;
	uint32 addr;

#if __FPGA
	uint32 defaultValue[] = {
		0x00000002, 0xff130000, 0x02d00500, 0x02d00500,				// 0x0000
		0x001e07e4, 0x00000072, 0x05a005a0, 0x05a005a0,				// 0x0010
		0x00000000, 0x20040000, 0x00000040, 0x00000000,				// 0x0020
		0x20040000, 0x203c4000, 0x20748000, 0x20acc000,				// 0x0030
		0x01312d00, 0x01312d00, 0x20020000, 0x20020000,				// 0x0040
		0x00000005, 0x00005aa1, 0x10000007, 0x01000100,				// 0x0050
		0x0258012c, 0x05dc04b0, 0x20e50000, 0x009703ec,				// 0x0060
		0x016700ac, 0x40ff554c, 0x00000000, 0x00008080,				// 0x0070
		0x03670000, 0x024e0000, 0x018f0370, 0x02cf0000,				// 0x0080
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0090
		0x01f49632, 0x01f49632, 0x01f49632, 0x01f49632,				// 0x00a0
		0x00000000, 0x00000000, 0x20e90000, 0x00000000,				// 0x00b0
		0x00000006, 0x00000000, 0x00000000, 0x00000000,				// 0x00c0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00d0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00e0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00f0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0100
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0110
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0120
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0130
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0140
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0150
		0x40404040, 0x40404040, 0x40404040, 0x40404040,				// 0x0160
		0x40404040, 0x40404040, 0x40404040, 0x40404040,				// 0x0170
		0x40404040, 0x011d964d, 0x0080552c, 0x00156b80,				// 0x0180
		0x0000b758, 0x016701c6, 0xffffffff							// 0x0190
	};
#endif

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(defaultValue)/4; i++ )
	{
		if ( (addr != 0xf0800024) && ((addr <= 0xf08000bc) || (addr >= 0xf0800160 && addr <= 0xf0800198)) )
		{
			*((vuint32*)(addr)) = defaultValue[i];
		}
		addr += 4;
	}
}

/***************************************************************************************************************************************************************
 * register default value
***************************************************************************************************************************************************************/
static void svmtest_regDefault(void)
{
	int i;
	uint32 addr;
	int total = 0, error = 0;

#if __FPGA
	uint32 defaultValue[] = {
		0x00000002, 0xff130000, 0x02d00500, 0x02d00500,				// 0x0000
		0x001e07e4, 0x00000072, 0x05a005a0, 0x05a005a0,				// 0x0010
		0x00000000, 0x20040000, 0x00000040, 0x00000000,				// 0x0020
		0x20040000, 0x203c4000, 0x20748000, 0x20acc000,				// 0x0030
		0x01312d00, 0x01312d00, 0x20020000, 0x20020000,				// 0x0040
		0x00000005, 0x00005aa1, 0x10000007, 0x01000100,				// 0x0050
		0x0258012c, 0x05dc04b0, 0x20e50000, 0x009703ec,				// 0x0060
		0x016700ac, 0x40ff554c, 0x00000000, 0x00008080,				// 0x0070
		0x03670000, 0x024e0000, 0x018f0370, 0x02cf0000,				// 0x0080
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0090
		0x01f49632, 0x01f49632, 0x01f49632, 0x01f49632,				// 0x00a0
		0x00000000, 0x00000000, 0x20e90000, 0x00000000,				// 0x00b0
		0x00000006, 0x00000000, 0x00000000, 0x00000000,				// 0x00c0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00d0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00e0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00f0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0100
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0110
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0120
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0130
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0140
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0150
		0x40404040, 0x40404040, 0x40404040, 0x40404040,				// 0x0160
		0x40404040, 0x40404040, 0x40404040, 0x40404040,				// 0x0170
		0x40404040, 0x011d964d, 0x0080552c, 0x00156b80,				// 0x0180
		0x0000b758, 0x016701c6, 0xffffffff							// 0x0190
	};
#endif

	LOG("********************************************************\n");
	LOG("* [ default value of register ]\n");

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(defaultValue)/4; i++ )
	{
		if ( (addr <= 0xf08000c0) || (addr >= 0xf0800100 && addr <= 0xf0800198) )
		{
			total++;
			if ( *((vuint32*)(addr)) != defaultValue[i] )
			{
				error++;
				LOG("0x%08x failed\n", addr);
			}
		}

		addr += 4;
	}

	if ( error == 0 )
	{
		LOG("[ success ]\n\n");
	}
	else
	{
		LOG("[ total(%d), error(%d) ]\n\n", total, error);
	}
}
/***************************************************************************************************************************************************************
 * register write & read value
***************************************************************************************************************************************************************/
static void svmtest_regWR(void)
{
	int i;
	uint32 addr;
	int total[2] = {0,}, error[2] = {0,};

	uint32 wrValue[] = {
		0x00000007, 0xffffffff, 0x07ff07ff, 0x07ff07ff,				// 0x0000
		0xffffffff, 0x000007f7, 0x1fff1fff, 0x1fff1fff,				// 0x0010
		0x00000000, 0x00000000, 0x000001f3, 0x00000000,				// 0x0020
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,				// 0x0030
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,				// 0x0040
		0xffffff0f, 0xffffffff, 0xffff00ff, 0x07ff07ff,				// 0x0050
		0x07ff07ff, 0x07ff07ff, 0xffffffff, 0x01ff07ff,				// 0x0060
		0x81ff07ff, 0x7fffffff, 0x00ffffff, 0x00ffffff,				// 0x0070
		0x07ff07ff, 0x87ff07ff, 0x07ff07ff, 0x87ff07ff,				// 0x0080
		0x87ff07ff, 0x87ff07ff, 0x87ff07ff, 0x87ff07ff,				// 0x0090
		0x87ffff0f, 0x87ffff0f, 0x87ffff0f, 0x87ffff0f,				// 0x00a0
		0xffffffff, 0xffffffff, 0xffffffff, 0x000000ff,				// 0x00b0

		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00c0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00d0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00e0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x00f0
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0100
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0110
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0120
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0130
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0140
		0x00000000, 0x00000000, 0x00000000, 0x00000000,				// 0x0150

		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,				// 0x0160
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,				// 0x0170
		0xffffffff, 0x01ffffff, 0x00ffffff, 0x00ffffff,				// 0x0180
		0x0000ffff, 0x03ff03ff, 0xffffffff							// 0x0190
	};

	LOG("********************************************************\n");
	LOG("* [ WR of register ]\n");

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(wrValue)/4; i++ )
	{
		if ( (addr != 0xf0800024) && ((addr <= 0xf08000bc) || (addr >= 0xf0800160 && addr <= 0xf0800198)) )
		{
			*((vuint32*)(addr)) = 0x00000000;
		}

		addr += 4;
	}

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(wrValue)/4; i++ )
	{
		if ( (addr != 0xf0800024) && ((addr <= 0xf08000bc) || (addr >= 0xf0800160 && addr <= 0xf0800198)) )
		{
			total[0]++;
			if ( *((vuint32*)(addr)) !=  0x00000000 )
			{
				error[0]++;
				LOG("0x%08x failed\n", addr);
			}
		}

		addr += 4;
	}

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(wrValue)/4; i++ )
	{
		if ( (addr != 0xf0800024) && ((addr <= 0xf08000bc) || (addr >= 0xf0800160 && addr <= 0xf0800198)) )
		{
			*((vuint32*)(addr)) = wrValue[i];
		}

		addr += 4;
	}

	addr = SVM_BASE_ADDR;
	for ( i = 0; i < sizeof(wrValue)/4; i++ )
	{
		if ( (addr != 0xf0800024) && ((addr <= 0xf08000bc) || (addr >= 0xf0800160 && addr <= 0xf0800198)) )
		{
			total[1]++;
			if ( *((vuint32*)(addr)) !=  wrValue[i] )
			{
				error[1]++;
				LOG("0x%08x failed\n", addr);
			}
		}

		addr += 4;
	}

	if ( error[0] == 0 && error[1] == 0 )
	{
		LOG("[ success ]\n\n");
	}
	else
	{
		LOG("[ total(%d), error(%d) ]\n\n", total[0], error[0] + error[1]);
	}
}

/***************************************************************************************************************************************************************
 * interrupt
***************************************************************************************************************************************************************/
static uint64 gSVMInrtCnt = 0;

static void svmtest_cbVsync(void)
{
	gSVMInrtCnt++;
}

static void svmtest_interrupt(void)
{
	int i = 0;

	LOG("********************************************************\n");
	LOG("* [ interrupt ]\n");

	svmtest_setRegInit();

	SVMAPI_setCBOutputVsync(svmtest_cbVsync);

	for ( i = 0; i < 100; i++ )
	{
		if ( gSVMInrtCnt >= 20 ) break;
		svmtest_sleep(100);
	}

	SVMAPI_setCBOutputVsync(NULL);

	if ( gSVMInrtCnt >= 20 )
	{
		LOG("[ success ]\n\n");
	}
	else
	{
		LOG("[ faild ]\n\n");
	}
}

/***************************************************************************************************************************************************************
 * svm disable
***************************************************************************************************************************************************************/
static void svmtest_svmDisable()
{
	LOG("********************************************************\n");
	LOG("* [ disable svm ]\n");

	svmtest_setRegInit();

	svmtest_sleep(5000);
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * bypass + flipping
***************************************************************************************************************************************************************/
static void svmtest_bypassNflipping(void)
{
	int i;
	uint8 viewMode[eSVM_CHANNEL_MAX];
	char channelName[32];

	LOG("********************************************************\n");
	LOG("* [ bypass & flipping ]\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
	{
		SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, i, &viewMode[i]);
	}

	for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
	{
		switch (i)
		{
		case eSVM_CHANNEL_FRONT:	strcpy(channelName, "front");	break;
		case eSVM_CHANNEL_LEFT:		strcpy(channelName, "left");	break;
		case eSVM_CHANNEL_RIGHT:	strcpy(channelName, "right");	break;
		case eSVM_CHANNEL_BACK:		strcpy(channelName, "back");	break;
		}

		LOG("* %s bypass - non flipping\n", channelName);
		SVMAPI_setChannelFilpping(i, FALSE, FALSE);
		SVMAPI_setViewMode(viewMode[i], FALSE, FALSE);
		svmtest_sleep(5000);

		LOG("* %s bypass - horizontal flipping\n", channelName);
		SVMAPI_setChannelFilpping(i, TRUE, FALSE);
		svmtest_sleep(5000);

		LOG("* %s bypass - vertical flipping\n", channelName);
		SVMAPI_setChannelFilpping(i, FALSE, TRUE);
		svmtest_sleep(5000);

		LOG("* %s bypass - horizontal & vertical flipping\n", channelName);
		SVMAPI_setChannelFilpping(i, TRUE, TRUE);
		svmtest_sleep(5000);
	}

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * enable or disable of input & output
***************************************************************************************************************************************************************/
static void svmtest_inOutEnableDisable(void)
{
	int i;
	uint8 viewMode;
	SVM_RECT_T section[2];
	SVM_BOUNDARY_T hscale;

	LOG("********************************************************\n");
	LOG("* [ enable or disable input & output ]\n");
	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	section[0].x = 0;
	section[0].y = 0;
	section[0].width = 872;
	section[0].height = 591;
	section[1].x = 880;
	section[1].y = 0;
	section[1].width = 400;
	section[1].height = 720;

	hscale.area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
	hscale.area[eSVM_BOUNDARYAEAR_0].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_1].startYLine = 200;
	hscale.area[eSVM_BOUNDARYAEAR_1].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_2].startYLine = 300;
	hscale.area[eSVM_BOUNDARYAEAR_2].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_3].startYLine = 500;
	hscale.area[eSVM_BOUNDARYAEAR_3].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;

	SVMAPI_createViewMode(SVM_USE_LUT, SVM_USE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_LUT_HALF_BLEND, &viewMode);

	for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
	{
		SVMAPI_setSection(viewMode, i, section[i]);
		SVMAPI_setSectionEnable(viewMode, i, TRUE);
	}

#ifdef __FPGA
	SVMAPI_setCompression(viewMode, eSVM_COMPRESSION_RATIO_HALF);
	SVMAPI_setCompressionEnable(viewMode, TRUE);

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		SVMAPI_setHorizontalScale(viewMode, i, hscale);
		SVMAPI_setVerticalHalfScale(viewMode, i, TRUE);
	}
#endif

	SVMAPI_updateFBLUT(viewMode, svm_test_LUT4FrontBack, sizeof(svm_test_LUT4FrontBack));
	SVMAPI_updateLRLUT(viewMode, svm_test_LUT4LeftRight, sizeof(svm_test_LUT4LeftRight));

	SVMAPI_setViewMode(viewMode, FALSE, FALSE);

	LOG("* clear input channel\n");
	SetRegValue(0xf01000e8, 0x80);			// test pattern black
	svmtest_sleep(8000);

	LOG("* Input : disable front, left, right, back\n");
	SVMAPI_setInputEnable(FALSE, FALSE, FALSE, FALSE);
	svmtest_sleep(3000);
	SetRegValue(0xf01000e8, 0x0);			// test pattern off

	LOG("* Input : enable front\n");
	SVMAPI_setInputEnable(TRUE, FALSE, FALSE, FALSE);
	svmtest_sleep(3000);

	LOG("* Input : enable front, left\n");
	SVMAPI_setInputEnable(TRUE, TRUE, FALSE, FALSE);
	svmtest_sleep(3000);

	LOG("* Input : enable front, left, right\n");
	SVMAPI_setInputEnable(TRUE, TRUE, TRUE, FALSE);
	svmtest_sleep(3000);

	LOG("* Input : enable front, left, right, back\n");
	SVMAPI_setInputEnable(TRUE, TRUE, TRUE, TRUE);
	svmtest_sleep(3000);

	LOG("* output : disable fb lr\n");
	SVMAPI_setOutputEnable(FALSE, FALSE);
	svmtest_sleep(3000);

#if 0
	LOG("* output : enable fb\n");
	SVMAPI_setOutputEnable(TRUE, FALSE);
	svmtest_sleep(3000);

	LOG("* output : enable lr\n");
	SVMAPI_setOutputEnable(FALSE, TRUE);
	svmtest_sleep(3000);
#endif

	LOG("* output : enable fb lr\n");
	SVMAPI_setOutputEnable(TRUE, TRUE);
	svmtest_sleep(3000);

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * look up table
***************************************************************************************************************************************************************/
static void svmtest_lut(void)
{
	int i;
	uint8 viewMode;
	SVM_RECT_T section[2];
	SVM_BOUNDARY_T hscale;

	LOG("********************************************************\n");
	LOG("* [ LUT ]\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	hscale.area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
	hscale.area[eSVM_BOUNDARYAEAR_0].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_1].startYLine = 200;
	hscale.area[eSVM_BOUNDARYAEAR_1].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_2].startYLine = 300;
	hscale.area[eSVM_BOUNDARYAEAR_2].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_3].startYLine = 500;
	hscale.area[eSVM_BOUNDARYAEAR_3].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;

	SVMAPI_createViewMode(SVM_USE_LUT, SVM_USE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_LUT_HALF_BLEND, &viewMode);

	section[0].x = 0;
	section[0].y = 0;
	section[0].width = 872;
	section[0].height = 591;
	section[1].x = 880;
	section[1].y = 0;
	section[1].width = 400;
	section[1].height = 720;
	for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
	{
		SVMAPI_setSection(viewMode, i, section[i]);
		SVMAPI_setSectionEnable(viewMode, i, TRUE);
	}

#ifdef __FPGA
	SVMAPI_setCompression(viewMode, eSVM_COMPRESSION_RATIO_HALF);
	SVMAPI_setCompressionEnable(viewMode, TRUE);

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		SVMAPI_setHorizontalScale(viewMode, i, hscale);
		SVMAPI_setVerticalHalfScale(viewMode, i, TRUE);
	}
#endif

	SVMAPI_updateFBLUT(viewMode, svm_test_LUT4FrontBack, sizeof(svm_test_LUT4FrontBack));
	SVMAPI_updateLRLUT(viewMode, svm_test_LUT4LeftRight, sizeof(svm_test_LUT4LeftRight));

	SVMAPI_setViewMode(viewMode, FALSE, FALSE);
	svmtest_sleep(5000);

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * in size & out size
***************************************************************************************************************************************************************/
static void svmtest_diffInOutSize(void)
{
	uint8 viewMode;

	LOG("********************************************************\n");
	LOG("* [ diiferent in/out size ]\n");

	LOG("* 1280x720 -> 720x480\n");
	svmtest_setRegInit();
	SVMAPI_initialize(1280, 720, 720, 480, 8, 3);
	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);
	SVMAPI_setViewMode(viewMode, FALSE, FALSE);
	svmtest_sleep(5000);
	SVMAPI_deInitialize();

	LOG("* 1280x720 -> 720x576\n");
	svmtest_setRegInit();
	SVMAPI_initialize(1280, 720, 720, 576, 8, 3);
	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);
	SVMAPI_setViewMode(viewMode, FALSE, FALSE);
	svmtest_sleep(5000);
	SVMAPI_deInitialize();

	LOG("* 1280x720 -> 1920x1080\n");
	svmtest_setRegInit();
	SetRegValue(0xf0800000, 0x2);
	SVMAPI_initialize(1280, 720, 1920, 1080, 8, 3);
	SVMAPI_setBlank(740, 60);
	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);
	SVMAPI_setViewMode(viewMode, FALSE, FALSE);
	svmtest_sleep(5000);
	SVMAPI_deInitialize();

	LOG("\n");
}

/***************************************************************************************************************************************************************
 * input dram
***************************************************************************************************************************************************************/
static void svmtest_inputDram(void)
{
	int i, k;
	int vScale;
	uint8 viewMode[eSVM_CHANNEL_MAX];
	uint32 vScaleNoAddr[eSVM_CHANNEL_MAX], vScaleYesAddr[eSVM_CHANNEL_MAX];
	uint8 channel;
	char channelName[32];

	LOG("********************************************************\n");
	LOG("* [ input drame - bypass ]\n");
	LOG("* The input data of each channel must be updated.\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 1);

	for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
	{
		SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, i, &viewMode[i]);
	}

	SVMAPI_setInputTestModeEnable(TRUE);
	SVMAPI_setInputWrappingInitEnable(TRUE);

	SVMAPI_getChannelInputAddr(&vScaleNoAddr[eSVM_CHANNEL_FRONT], &vScaleNoAddr[eSVM_CHANNEL_LEFT], &vScaleNoAddr[eSVM_CHANNEL_RIGHT], &vScaleNoAddr[eSVM_CHANNEL_BACK]);
	for ( i = eSVM_CHANNEL_FRONT; i < eSVM_CHANNEL_MAX; i++ )
	{
		vScaleYesAddr[i] = vScaleNoAddr[i] + 0x00800000;
	}

	LOG("*  - restore imagefilename binary memoryAddr\n");
	LOG("*   <no vScale>\n");
	LOG("*   front	: restore front_img_vscl0.bin binary 0x%08x\n", vScaleNoAddr[0]);
	LOG("*   left	: restore left_img_vscl0.bin binary 0x%08x\n", vScaleNoAddr[1]);
	LOG("*   right	: restore right_img_vscl0.bin binary 0x%08x\n", vScaleNoAddr[2]);
	LOG("*   back	: restore back_img_vscl0.bin binary 0x%08x\n", vScaleNoAddr[3]);
	LOG("*   <half vScale>\n");
	LOG("*   front	: restore front_img_vscl1.bin binary 0x%08x\n", vScaleYesAddr[0]);
	LOG("*   left	: restore left_img_vscl1.bin binary 0x%08x\n", vScaleYesAddr[1]);
	LOG("*   right	: restore right_img_vscl1.bin binary 0x%08x\n", vScaleYesAddr[2]);
	LOG("*   back	: restore back_img_vscl1.bin binary 0x%08x\n", vScaleYesAddr[3]);
	LOG("* Please... Ctrl + c. and rerun after updating.\n");
	svmtest_sleep(10000);

	for ( vScale = 0; vScale < 2; vScale++ )
	{
		char strVscale[32];

		if ( vScale == 0 )
			strcpy(strVscale, "disable vscale");
		else
			strcpy(strVscale, "enable vscale");

		for ( k = eSVM_CHANNEL_FRONT; k < eSVM_CHANNEL_MAX; k++ )
		{
			if ( vScale == 0 )
			{
				SVMAPI_setChannelInputAddr(k, vScaleNoAddr[k]);
			}
			else
			{
				SVMAPI_setChannelInputAddr(k, vScaleYesAddr[k]);
			}
		}

		for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
		{
			for ( k = eSVM_CHANNEL_FRONT; k < eSVM_CHANNEL_MAX; k++ )
			{
				SVMAPI_setVerticalHalfScale(viewMode[i], k, vScale);
			}
		}

		for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
		{
			channel = i;	// tagSVM_CHANNEL_FRONT
			switch (channel)
			{
			case eSVM_CHANNEL_FRONT:	strcpy(channelName, "front");	break;
			case eSVM_CHANNEL_LEFT:		strcpy(channelName, "left");	break;
			case eSVM_CHANNEL_RIGHT:	strcpy(channelName, "right");	break;
			case eSVM_CHANNEL_BACK:		strcpy(channelName, "back");	break;
			}

			LOG("* %d). %s bypass - %s\n", i+1, channelName, strVscale);
			SVMAPI_setViewMode(viewMode[i], FALSE, FALSE);
			svmtest_sleep(5000);
		}
	}

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * bypass + car
***************************************************************************************************************************************************************/
static void svmtest_car(void)
{
	int i;
	uint8 viewMode;
	uint16 outWidth = 1920, outHeight = 1080;
	SVM_BOUNDARY_T hscale;
	SVM_RECT_T rectCar = {(outWidth - 360) / 2, (outHeight - 360) / 2, 360, 360};
	SVM_RECT_T section[2];
	uint16 pos[] = {
		0, 0,
		outWidth - rectCar.width, 0,
		0, outHeight - rectCar.height,
		outWidth - rectCar.width, outHeight - rectCar.height,
		outWidth + 100, outHeight + 100
	};
	uint16 size[] = {
		2, 1,
		2, 2,
		10, 10,
		100, 100,
		200, 200,
		300, 300,
		400, 400,
		500, 500,
		600, 600,
		700, 700,
		800, 800,
		900, 900,
		1000, 1000,
		1100, 1080,
		1200, 1080,
		1300, 1080,
		1400, 1080,
		1500, 1080,
		1600, 1080,
		1700, 1080,
		1800, 1080,
		1920, 1080,
	};

	LOG("********************************************************\n");
	LOG("* [ Car ]\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, outWidth, outHeight, 8, 3);
	section[0].x = 0;
	section[0].y = 0;
	section[0].width = 872;
	section[0].height = 591;
	section[1].x = 880;
	section[1].y = 0;
	section[1].width = 400;
	section[1].height = 720;

	hscale.area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
	hscale.area[eSVM_BOUNDARYAEAR_0].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_1].startYLine = 200;
	hscale.area[eSVM_BOUNDARYAEAR_1].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_2].startYLine = 300;
	hscale.area[eSVM_BOUNDARYAEAR_2].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_3].startYLine = 500;
	hscale.area[eSVM_BOUNDARYAEAR_3].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;

	SVMAPI_createViewMode(SVM_USE_LUT, SVM_USE_LUT, SVM_UNUSE_LUT, SVM_USE_CAR, eSVM_OUTPUTMODE_LUT_HALF_BLEND, &viewMode);

	for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
	{
		SVMAPI_setSection(viewMode, i, section[i]);
		SVMAPI_setSectionEnable(viewMode, i, TRUE);
	}

#ifdef __FPGA
	SVMAPI_setCompression(viewMode, eSVM_COMPRESSION_RATIO_HALF);
	SVMAPI_setCompressionEnable(viewMode, TRUE);

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		SVMAPI_setHorizontalScale(viewMode, i, hscale);
		SVMAPI_setVerticalHalfScale(viewMode, i, TRUE);
	}
#endif

	SVMAPI_updateFBLUT(viewMode, svm_test_LUT4FrontBack, sizeof(svm_test_LUT4FrontBack));
	SVMAPI_updateLRLUT(viewMode, svm_test_LUT4LeftRight, sizeof(svm_test_LUT4LeftRight));
#if 0
	SVMAPI_updateCar(viewMode, (uint16*)svm_test_carImage, sizeof(svm_test_carImage));
#else
	// update car image
	{
		uint32 *p;
		p = (uint32*)0x22000000;	// temp memory address
		for ( i = 0; i < (outWidth * outHeight * 2) / 4; i++ )
		{
			*(p+i) = 0xFF4C554C;	// Red color
		}

		SVMAPI_setCarAddr(viewMode, 0x22000000);
	}
#endif

	SVMAPI_setViewMode(viewMode, FALSE, FALSE);

	/*****************************************************************************************************
	* Enable / Disable
	*****************************************************************************************************/
	SVMAPI_setCar(viewMode, rectCar, 0x0, 0x0, 0x0);
	LOG("* Enable\n");
	SVMAPI_setCarEnable(viewMode, TRUE);
	svmtest_sleep(5000);

	LOG("* Disable\n");
	SVMAPI_setCarEnable(viewMode, FALSE);
	svmtest_sleep(5000);

	LOG("* Enable\n");
	SVMAPI_setCarEnable(viewMode, TRUE);
	svmtest_sleep(5000);

	/*****************************************************************************************************
	* mask color
	*****************************************************************************************************/
	LOG("* Mask color : Red\n");
	SVMAPI_setCar(viewMode, rectCar, 0x4C, 0x55, 0xFF);
	svmtest_sleep(5000);
	SVMAPI_setCar(viewMode, rectCar, 0x0, 0x0, 0x0);

	/*****************************************************************************************************
	* position
	*****************************************************************************************************/
	for ( i = 0; i < sizeof(pos) / sizeof(uint16); i+=2 )
	{
		rectCar.x = pos[i+0];
		rectCar.y = pos[i+1];
		LOG("* Move (%d, %d)\n", rectCar.x, rectCar.y);
		SVMAPI_setCar(viewMode, rectCar, 0x0, 0x0, 0x0);
		svmtest_sleep(5000);
	}

	/*****************************************************************************************************
	* size
	*****************************************************************************************************/
	rectCar.x = 0;
	rectCar.y = 0;

	SVMAPI_setCarEnable(viewMode, TRUE);

	for ( i = 0; i < sizeof(size) / sizeof(uint16); i+=2 )
	{
		rectCar.width = size[i+0];
		rectCar.height = size[i+1];
		LOG("* Size (%d, %d)\n", rectCar.width, rectCar.height);
		SVMAPI_setCar(viewMode, rectCar, 0x0, 0x0, 0x0);
		svmtest_sleep(5000);
	}

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * section
***************************************************************************************************************************************************************/
static void svmtest_section(void)
{
	int i, k;
	uint8 viewMode;
	SVM_RECT_T section[2];
	uint16 outWidth = 1280, outHeight = 720;
	uint16 pos[] = {
		0, 0, outWidth - 200, 0,
		outWidth - 200, 0, outWidth - 200, outHeight - 200,
		outWidth - 200, outHeight - 200, 0, outHeight - 200,
		0, outHeight - 200, 0, 0,
		outWidth + 100, outHeight + 100, outWidth + 100, outHeight + 100
	};

	uint16 size[] = {0, 1, 2, 55, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, outWidth};

	LOG("********************************************************\n");
	LOG("* [ Section ]\n");

	svmtest_setRegInit();
	SVMAPI_initialize(1280, 720, outWidth, outHeight, 8, 3);
	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);
	SVMAPI_setViewMode(viewMode, FALSE, FALSE);

	//SVMAPI_setOutputEnable(TRUE, TRUE);		// __temp

	/*****************************************************************************************************
	* Enable / Disable
	*****************************************************************************************************/
	section[0].x = 0;
	section[0].y = 0;
	section[0].width = outWidth / 2;
	section[0].height = outHeight / 2;
	section[1].x = outWidth / 2;
	section[1].y = outHeight / 2;
	section[0].width = outWidth / 2;
	section[0].height = outHeight / 2;
	LOG("* Enable 0 & 1\n");
	for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
	{
		SVMAPI_setSection(viewMode, i, section[i]);
		SVMAPI_setSectionEnable(viewMode, i, TRUE);
	}
	svmtest_sleep(5000);

	LOG("* Disable 0\n");
	SVMAPI_setSectionEnable(viewMode, 0, FALSE);
	svmtest_sleep(5000);

	LOG("* Disable 0 & 1\n");
	SVMAPI_setSectionEnable(viewMode, 1, FALSE);
	svmtest_sleep(5000);

	/*****************************************************************************************************
	* Position
	*****************************************************************************************************/
	section[0].width = 200;
	section[0].height = 200;
	section[1].width = 200;
	section[1].height = 200;

	for ( k = 0; k < sizeof(pos) / sizeof(uint16); k+=4 )
	{
		section[0].x = pos[k+0];
		section[0].y = pos[k+1];
		section[1].x = pos[k+2];
		section[1].y = pos[k+3];
		LOG("* Move : section0 (%d, %d) - section1 (%d, %d)\n", section[0].x, section[0].y, section[1].x, section[1].y);
		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			SVMAPI_setSection(viewMode, i, section[i]);
			SVMAPI_setSectionEnable(viewMode, i, TRUE);
		}
		svmtest_sleep(5000);
	}

	/*****************************************************************************************************
	* Size
	*****************************************************************************************************/
	section[0].x = 0;
	section[0].y = 0;
	section[1].x = 100;
	section[1].y = 100;

	for ( k = 0; k < sizeof(size) / sizeof(uint16); k++ )
	{
		section[0].width = size[k];
		section[0].height = size[k];
		section[1].width = size[k];
		section[1].height = size[k];
		LOG("* Size : section0 (%d, %d) - section1 (%d, %d)\n", section[0].width, section[0].height, section[1].width, section[1].height);
		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			SVMAPI_setSection(viewMode, i, section[i]);
			SVMAPI_setSectionEnable(viewMode, i, TRUE);
		}
		svmtest_sleep(5000);
	}

	SVMAPI_deInitialize();
}

/***************************************************************************************************************************************************************
 * Crop
***************************************************************************************************************************************************************/
static void svmtest_crop(void)
{
	int i, k;
	uint8 viewMode[eSVM_CHANNEL_MAX];
	char channelName[32];
	SVM_ROI_AREA_T roi;
	uint16 pos[] = {0, 1, 2, 10, 100, 200, 300, 400, 500, 600, 700, 800};
	uint16 size[] = {0, 1, 2, 10, 100, 200, 300, 400, 500, 600, 700, 800};

	LOG("********************************************************\n");
	LOG("* [ Crop ]\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
	{
		SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, i, &viewMode[i]);
	}

	for ( i = eSVM_OUTPUTMODE_BYPASS_FRONT; i <= eSVM_OUTPUTMODE_BYPASS_BACK; i++ )
	{
		switch (i)
		{
		case eSVM_CHANNEL_FRONT:	strcpy(channelName, "front");	break;
		case eSVM_CHANNEL_LEFT:		strcpy(channelName, "left");	break;
		case eSVM_CHANNEL_RIGHT:	strcpy(channelName, "right");	break;
		case eSVM_CHANNEL_BACK:		strcpy(channelName, "back");	break;
		}

		SVMAPI_setViewMode(viewMode[i], FALSE, FALSE);
		//SVMAPI_setOutputEnable(TRUE, TRUE);		// __temp

		/*****************************************************************************************************
		* Enable / Disable
		*****************************************************************************************************/
		roi.startYLine = 0;
		roi.endYLine = 200;
		LOG("* %s Enable\n", channelName);
		SVMAPI_setROI(viewMode[i], i, roi);
		SVMAPI_setROIEnable(viewMode[i], i, TRUE);
		svmtest_sleep(5000);

		LOG("* %s Disable\n", channelName);
		SVMAPI_setROIEnable(viewMode[i], i, FALSE);
		svmtest_sleep(5000);

		LOG("* %s Enable\n", channelName);
		SVMAPI_setROIEnable(viewMode[i], i, TRUE);
		svmtest_sleep(5000);

		/*****************************************************************************************************
		* Position
		*****************************************************************************************************/
		for ( k = 0; k < sizeof(pos) / sizeof(uint16); k++ )
		{
			roi.startYLine = pos[k];
			roi.endYLine = pos[k] + 100;
			LOG("* %s Position (%d, %d)\n", channelName, roi.startYLine, roi.endYLine);
			SVMAPI_setROI(viewMode[i], i, roi);
			SVMAPI_setROIEnable(viewMode[i], i, TRUE);
			svmtest_sleep(5000);
		}

		/*****************************************************************************************************
		* Size
		*****************************************************************************************************/
		roi.startYLine = 0;
		for ( k = 0; k < sizeof(size) / sizeof(uint16); k++ )
		{
			roi.endYLine = size[k];
			LOG("* %s Size (%d, %d)\n", channelName, roi.startYLine, roi.endYLine);
			SVMAPI_setROI(viewMode[i], i, roi);
			SVMAPI_setROIEnable(viewMode[i], i, TRUE);
			svmtest_sleep(5000);
		}
	}

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * bypass + vscale + hscale + flipping
***************************************************************************************************************************************************************/
static void svmtest_bypassScenario(void)
{
	uint8 viewMode;
	SVM_BOUNDARY_T hscale;

	LOG("********************************************************\n");
	LOG("* [ Crop ]\n");

	svmtest_setRegInit();

	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);

	SVMAPI_setViewMode(viewMode, FALSE, FALSE);
	//SVMAPI_setOutputEnable(TRUE, TRUE);		// __temp

	//SVMAPI_setCompression(viewMode, eSVM_COMPRESSION_RATIO_HALF);
	//SVMAPI_setCompressionEnable(viewMode, TRUE);

	/*****************************************************************************************************
	* vscale + flipping
	*****************************************************************************************************/
	LOG("* Vertical scale\n");
	SVMAPI_setVerticalHalfScale(viewMode, eSVM_CHANNEL_FRONT, TRUE);
	svmtest_sleep(5000);

	LOG("* Vertical scale + Horizontal flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, FALSE);
	svmtest_sleep(5000);

	LOG("* Vertical scale + Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, FALSE, TRUE);
	svmtest_sleep(5000);

	LOG("* Vertical scale + Horizontal/Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, TRUE);
	svmtest_sleep(5000);

	SVMAPI_setVerticalHalfScale(viewMode, eSVM_CHANNEL_FRONT, FALSE);
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, FALSE, FALSE);

	/*****************************************************************************************************
	* hscale + flipping
	*****************************************************************************************************/
	LOG("* Horizontal scale\n");
	hscale.area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
	hscale.area[eSVM_BOUNDARYAEAR_0].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_1].startYLine = 200;
	hscale.area[eSVM_BOUNDARYAEAR_1].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_2].startYLine = 300;
	hscale.area[eSVM_BOUNDARYAEAR_2].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	hscale.area[eSVM_BOUNDARYAEAR_3].startYLine = 500;
	hscale.area[eSVM_BOUNDARYAEAR_3].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
	SVMAPI_setHorizontalScale(viewMode, eSVM_CHANNEL_FRONT, hscale);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Horizontal flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, FALSE);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, FALSE, TRUE);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Horizontal/Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, TRUE);
	svmtest_sleep(5000);

	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, FALSE, FALSE);

	/*****************************************************************************************************
	* hscale + vscale + flipping
	*****************************************************************************************************/
	LOG("* Horizontal scale + Vertical scale\n");
	SVMAPI_setVerticalHalfScale(viewMode, eSVM_CHANNEL_FRONT, TRUE);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Vertical scale + Horizontal flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, FALSE);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Vertical scale + Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, FALSE, TRUE);
	svmtest_sleep(5000);

	LOG("* Horizontal scale + Vertical scale + Horizontal/Vertical flipping\n");
	SVMAPI_setChannelFilpping(eSVM_CHANNEL_FRONT, TRUE, TRUE);
	svmtest_sleep(5000);

	SVMAPI_deInitialize();
	LOG("\n");
}

/***************************************************************************************************************************************************************
 * in size & out size
***************************************************************************************************************************************************************/
static void svmtest_outputDram(void)
{
	const uint32 outDramCtrl = SVM_BASE_ADDR + 0x000000e0;
	const uint32 outDramAddr = SVM_BASE_ADDR + 0x000000e4;
	bool isEnable;
	uint8 burstLength, holdTime;
	uint8 viewMode;

	LOG("********************************************************\n");
	LOG("* [ outputDram ]\n");

	svmtest_setRegInit();
	SVMAPI_initialize(1280, 720, 1280, 720, 8, 3);

	SVMAPI_createViewMode(SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_LUT, SVM_UNUSE_CAR, eSVM_OUTPUTMODE_BYPASS_FRONT, &viewMode);

	burstLength = 64;
	holdTime = 100;
	isEnable = TRUE;
	SetRegValue(outDramAddr, 0x23000000);
	SetRegValue(outDramCtrl, ((burstLength & utilBitMask(8)) << 0) | ((holdTime & utilBitMask(8)) << 8) | ((isEnable & utilBitMask(1)) << 31));

	SVMAPI_setViewMode(viewMode, FALSE, FALSE);

	svmtest_sleep(5000);

	SVMAPI_deInitialize();
}

/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_115200, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE, NULL);
	SYSAPI_DU_initialize();
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
int main()
{
	system_initialize();

#if 1
	SetRegValue(0xF01000EC, 0x20);		// sync_bt_sel
	SetRegValue(0xF01000F0, 0x8);		// YC Swap
#endif

	print_hex("a", 1);
	LOG("SVM Verification #############################################\n");

	//svmtest_regDefault();
	//svmtest_regWR();
	//svmtest_interrupt();
	//svmtest_svmDisable();
	//svmtest_bypassNflipping();
	//svmtest_diffInOutSize();
	svmtest_lut();
	//svmtest_inOutEnableDisable();
	//svmtest_inputDram();
	//svmtest_car();
	//svmtest_section();
	//svmtest_crop();
	//svmtest_bypassScenario();
	//svmtest_outputDram();

	while(1);

	return 1;
}
