#include "system.h"
#include "du.h"
#include "debug.h"


/****************** Temporal Define - To Do **************************************/
// BAYER QUEUE //
#define rBAYER_TOTAL_X		(ISP_BASE_ADDR + 0x4000)
#define rBAYER_TOTAL_Y		(ISP_BASE_ADDR + 0x4004)
#define rBAYER_START_X		(ISP_BASE_ADDR + 0x4008)
#define rBAYER_END_X		(ISP_BASE_ADDR + 0x400c)
#define rBAYER_START_Y		(ISP_BASE_ADDR + 0x4010)
#define rBAYER_END_Y		(ISP_BASE_ADDR + 0x4014)
#define rBAYER_KICK_X		(ISP_BASE_ADDR + 0x4018)
#define rBAYER_CONFIG		(ISP_BASE_ADDR + 0x401c)
/****************** Temporal Define - To Do **************************************/

#define BYPASS
/************************************************/
/************      DU Init         **************/
/************************************************/
/* 0.   Du_Init */
void du_init (void)
{
#if defined(BYPASS)
	SetRegValue(rDU4_CTRL, 0x11343000); // svm -> bto -> digital
	init_du4_bto_720p();
#else
#if defined(RES_FHD)
    init_du4_config_bypass_through_osd();
    init_du4_bto_1080p ();
	init_osd_ctrl_bypass ();
	init_osd_tg_1080p30();
	enable_atg_1080p30();      // 74.25MHz
	DU_PRINT("Du Init FHD Done!\n");
#elif defined(RES_HD)
    init_du4_config_bypass_through_osd();
	init_du4_bto_720p();
	init_osd_ctrl_bypass ();
	DU_PRINT("Mode Set!\n");
  	#ifdef COLOR_ROTATE
	  	#if defined(FPS_30)
	  	init_osd_tg_720p30();
	  	enable_atg_720p30();      //29.7MHz
	  	DU_PRINT("Du Init HD 30fps, CR Done!\n");
	  	#elif defined(FPS_60)	
	  	init_osd_tg_720p60();
	  	enable_atg_720p60();      //59.4Mhz
	  	DU_PRINT("Du Init HD 60fps, CR Done!\n");
	  	#endif
	#else
	  	init_osd_tg_720p60();
	  	enable_atg_720p60();
	  	DU_PRINT("Du Init HD 30/60fps, No-CR Done!\n");
	#endif
  	DU_PRINT("Du Init HD 30/60fps, No-CR Done!\n");
#endif
#endif
}

/* 1.   Configure Video Flow */
void init_du4_config_bypass_through_osd(void)
{
	SetRegValue(rDU4_CTRL, 0x45324001);	// svm -> pz -> osd -> atg ->  bto -> digital
	SetRegValue(rDU4_CTRL1, (0<<0));	// chroma_swap
}

/* 2.   Configure Mixer path & Layer order */
/* 2-1. isp -> Mixer(bypass) -> out */
void init_osd_ctrl_bypass(void)
{
	SetRegValue(rDU4_OSD_BUS_BW1, 0x003f3f3f);
	SetRegValue(rDU4_OSD_BUS_BW, 0x00001f1f);
	
	//  04: select what to route to 
	//          [3:0]:rgb_output, [7:4]:rle0, [11:8]:rle1, [15:12]:rle2, 
	//          [19:16]:font0, [23:20]:font1, [27:24]:yuv2rgb, [31:28]:rgb2yuv
	//      selectable sources
	//          0:rgb_input, 1:rle0, 2:rle1, 3:rle2, 4:font0, 5:font1, 6:yuv2rgb, 7:rgb2yuv
	SetRegValue(rDU4_OSD_SRC_SEL, 0x76543210);   // in -> out
	SetRegValue(rDU4_OSD_ENABLE, 0x0);
}

/* 3.   Control OSD TimingGenerate */
void init_osd_tg_1080p30(void)
{
	SetRegValue(rDU4_TG_SIZE, (1125 << 16) | 2200);
	SetRegValue(rDU4_TG_KICK, 279);						// 279 + sox
	SetRegValue(rDU4_TG_ACTIVE_X, (1920 << 16) | 1);	// 1~1920
	SetRegValue(rDU4_TG_ACTIVE_Y, (1079 << 16) | 0);	// 0~1079
	SetRegValue(rDU4_TG_CTRL, 0x1);						// 0:enable, 1:vsync(low_active), 2:ovsync(low_active), 3:ivsync,use_posedge
//	SetRegValue(rDU4_TG_CTRL, (0x1<<3) | 0x1);          // 0:enable, 1:vsync(low_active), 2:ovsync(low_active), 3:ivsync,use_posedge
}

void init_osd_tg_720p30(void)
{
//CR
	SetRegValue(rDU4_TG_SIZE, (750 << 16) | 1320);
	SetRegValue(rDU4_TG_SIZE, (750 << 16) | 1320);
	SetRegValue(rDU4_TG_KICK, 0);              			// 279 + sox
	SetRegValue(rDU4_TG_ACTIVE_X, (1280 << 16) | 1);	// 1~1280
	SetRegValue(rDU4_TG_ACTIVE_Y, (719 << 16) | 0);		// 0~719
	SetRegValue(rDU4_TG_CTRL, 0x1);						// 0:enable, 1:vsync(low_active), 2:ovsync(low_active), 3:ivsync,use_posedge
}

void init_osd_tg_720p60(void)
{
#ifdef COLOR_ROTATE
	SetRegValue(rDU4_TG_SIZE, (750 << 16) | 1320);
	SetRegValue(rDU4_TG_KICK, 128);              		// 279 + sox
	SetRegValue(rDU4_TG_ACTIVE_X, (1280 << 16) | 1);	// 1~1280
	SetRegValue(rDU4_TG_ACTIVE_Y, (719 << 16) | 0);		// 0~719
	SetRegValue(rDU4_TG_CTRL, 0x1);               		// 0:enable, 1:vsync(low_active), 2:ovsync(low_active), 3:ivsync,use_posedge
#else
//no-CR
	SetRegValue(rDU4_TG_SIZE, (750 << 16) | 1650);
	SetRegValue(rDU4_TG_KICK, 250);						// 279 + sox
	SetRegValue(rDU4_TG_ACTIVE_X, (1280 << 16) | 1);	// 1~1280
	SetRegValue(rDU4_TG_ACTIVE_Y, (719 << 16) | 0);		// 0~719
	SetRegValue(rDU4_TG_CTRL, 0x1);               		// 0:enable, 1:vsync(low_active), 2:ovsync(low_active), 3:ivsync,use_posedge
#endif
}

/* 4.   Control ATG */
void enable_atg_1080p30(void)       // 74.25MHz
{
	SetRegValue(rBAYER_TOTAL_X, 2200);
	SetRegValue(rBAYER_TOTAL_Y, 1125);
	SetRegValue(rBAYER_START_X, 1);
	SetRegValue(rBAYER_END_X, 1936);
	SetRegValue(rBAYER_START_Y, 0);
	SetRegValue(rBAYER_END_Y, 1095);
	SetRegValue(rBAYER_KICK_X, 480);

	SetRegValue(rDU4_ATG_TOTAL_X, 2200);
	SetRegValue(rDU4_ATG_TOTAL_Y, 1125);
	SetRegValue(rDU4_ATG_START_X, 280);
	SetRegValue(rDU4_ATG_END_X, (280 + 1920) - 1);
	SetRegValue(rDU4_ATG_START_Y, 0);
	SetRegValue(rDU4_ATG_END_Y, 1079);
	SetRegValue(rDU4_ATG_KICK_X, 256);		// kick + sox < (1024-16) expected
	
	//SetRegValue(rBAYER_CONFIG, 0b0011);		// Normal, moved to ispDrv.c
	SetRegValue(rDU4_ATG_CONFIG, 1);
}

void enable_atg_720p30(void)       // 29.7MHz
{
//CR
	SetRegValue(rBAYER_TOTAL_X, 1320);
	SetRegValue(rBAYER_TOTAL_Y, 750);
	SetRegValue(rBAYER_START_X, 1);
	SetRegValue(rBAYER_END_X, 1296);
	SetRegValue(rBAYER_START_Y, 0);
	SetRegValue(rBAYER_END_Y, 736 - 1);
//	SetRegValue(rBAYER_KICK_X, 256);
	SetRegValue(rBAYER_KICK_X, 1);

	SetRegValue(rDU4_ATG_TOTAL_X, 1650);
	SetRegValue(rDU4_ATG_TOTAL_Y, 750);
	SetRegValue(rDU4_ATG_START_X, 370);
	SetRegValue(rDU4_ATG_END_X, (370 + 1280) - 1);
	SetRegValue(rDU4_ATG_START_Y, 0);
	SetRegValue(rDU4_ATG_END_Y, 720 - 1);
	SetRegValue(rDU4_ATG_KICK_X, 1);            

	SetRegValue(rDU4_ATG_CONFIG, 1);
}

void enable_atg_720p60(void)       // 74.25MHz
{
#ifdef COLOR_ROTATE
//CR
	SetRegValue(rBAYER_TOTAL_X, 1320);
	SetRegValue(rBAYER_TOTAL_Y, 750);
	SetRegValue(rBAYER_START_X, 1);
	SetRegValue(rBAYER_END_X, 1296);
	SetRegValue(rBAYER_START_Y, 0);
	SetRegValue(rBAYER_END_Y, 736 - 1);
	SetRegValue(rBAYER_KICK_X, 256);

	SetRegValue(rDU4_ATG_TOTAL_X, 1650);
	SetRegValue(rDU4_ATG_TOTAL_Y, 750);
	SetRegValue(rDU4_ATG_START_X, 370);
	SetRegValue(rDU4_ATG_END_X, (370 + 1280) - 1);
	SetRegValue(rDU4_ATG_START_Y, 0);
	SetRegValue(rDU4_ATG_END_Y, 720 - 1);
	SetRegValue(rDU4_ATG_KICK_X, 512);		// kick + sox < (1024-16) expected
	
	//SetRegValue(rBAYER_CONFIG, 0b0011);		// Normal, moved to ispDrv.c
	SetRegValue(rDU4_ATG_CONFIG, 1);
#else
	SetRegValue(rBAYER_TOTAL_X, 1650);
	SetRegValue(rBAYER_TOTAL_Y, 750);
	SetRegValue(rBAYER_START_X, 1);
	SetRegValue(rBAYER_END_X, 1296);
	SetRegValue(rBAYER_START_Y, 0);
	SetRegValue(rBAYER_END_Y, 736 - 1);
	SetRegValue(rBAYER_KICK_X, 480);
	
	SetRegValue(rDU4_ATG_TOTAL_X, 1650);
	SetRegValue(rDU4_ATG_TOTAL_Y, 750);
	SetRegValue(rDU4_ATG_START_X, 1650 - 1280);
	SetRegValue(rDU4_ATG_END_X, 1650 - 1);
	SetRegValue(rDU4_ATG_START_Y, 0);
	SetRegValue(rDU4_ATG_END_Y, 720 - 1);
	SetRegValue(rDU4_ATG_KICK_X, 512);              // kick + sox < (1024-16) expected
	
	//SetRegValue(rBAYER_CONFIG, 0b0011);		// Normal, moved to ispDrv.c
	SetRegValue(rDU4_ATG_CONFIG, 1);
#endif
}

/* 5.   Configure BTO  */
void init_du4_bto_1080p(void)
{
	SetRegValue(rDU4_BTO_CTRL0, ((0 << 7) |		// SD_PAL_NTB = 1 : PAL, 0 : NTSC
								(0 << 6) |		// SD_960H_MD = 1 : 960H, 0 : 720H
								(0 << 5) |		// SD_OUTFMT_MD = 1 : SD, 0 : HD
								(0 << 4) |		// SD_FLD_POL = 1 : Odd High, 0 : Even High
								(1 << 3) |		// OUTFMT_16BIT = 1 : 16-bit, 0 : 8 Bit
								(0 << 2) |		// OUTFMT_BT656 = 1 : 656 Mode, 0 : 1120 Mode
								(0 << 1) |		// OUTFMT_YC_INV = 1 : YC_INV
								(1 << 0)) );		// BT1120_LIM_DIG = 1 : 16-235, 0 : 1-254
							
	SetRegValue(rDU4_BTO_CTRL1, ((0 << 7) |		// BTO_CLKO_POL = 1 : PAL, 0 : NTSC
								(7 << 4) |		// BTO_VAV_DEL = VAV Delay (Capture Option..)
								(0 << 0)) );		// BT1120_LIM_ANA = 1 : 16-235, 0 : 1-254
}

void init_du4_bto_720p(void)
{
	SetRegValue(rDU4_BTO_CTRL0, ((0 << 7) |		// SD_PAL_NTB = 1 : PAL, 0 : NTSC
								(0 << 6) |		// SD_960H_MD = 1 : 960H, 0 : 720H
								(0 << 5) |		// SD_OUTFMT_MD = 1 : SD, 0 : HD
								(0 << 4) |		// SD_FLD_POL = 1 : Odd High, 0 : Even High
								(1 << 3) |		// OUTFMT_16BIT = 1 : 16-bit, 0 : 8 Bit
								(0 << 2) |		// OUTFMT_BT656 = 1 : 656 Mode, 0 : 1120 Mode
								(0 << 1) |		// OUTFMT_YC_INV = 1 : YC_INV
								(1 << 0)) );		// BT1120_LIM_DIG = 1 : 16-235, 0 : 1-254

	SetRegValue(rDU4_BTO_CTRL1, ((0 << 7) |		// BTO_CLKO_POL = 1 : PAL, 0 : NTSC
								(7 << 4) |		// BTO_VAV_DEL = VAV Delay (Capture Option..)
								(0 << 0)) );		// BT1120_LIM_ANA = 1 : 16-235, 0 : 1-254
}

