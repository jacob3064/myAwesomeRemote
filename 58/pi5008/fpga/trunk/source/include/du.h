/* system.h */
#ifndef __DU_H__
#define __DU_H__

// DU

// DU4 CTRL //
#define rDU4_CTRL				(DU_BASE_ADDR + 0x0)
#define rDU4_CTRL1				(DU_BASE_ADDR + 0x4)
#define rDU4_STATE0				(DU_BASE_ADDR + 0x10)
#define rDU4_STATE1				(DU_BASE_ADDR + 0x14)
#define rDU4_STATE2				(DU_BASE_ADDR + 0x18)
#define rDU4_STATE3				(DU_BASE_ADDR + 0x1c)

// BTO CTRL //
#define rDU4_BTO_CTRL0			(DU_BASE_ADDR + 0x800)
#define rDU4_BTO_CTRL1			(DU_BASE_ADDR + 0x804)
#define rDU4_BTO_STATE			(DU_BASE_ADDR + 0x808)

// PZ CTRL //
#define rDU4_PZ_ZONE0			(DU_BASE_ADDR + 0x2000)
#define rDU4_PZ_ZONE1   		(DU_BASE_ADDR + 0x2004)
#define rDU4_PZ_ZONE2   		(DU_BASE_ADDR + 0x2008)
#define rDU4_PZ_ZONE3   		(DU_BASE_ADDR + 0x200c)
#define rDU4_PZ_ZONE4   		(DU_BASE_ADDR + 0x2010)
#define rDU4_PZ_ZONE5   		(DU_BASE_ADDR + 0x2014)
#define rDU4_PZ_ZONE6   		(DU_BASE_ADDR + 0x2018)
#define rDU4_PZ_ZONE7			(DU_BASE_ADDR + 0x201c)
#define rDU4_PZ_ENABLE			(DU_BASE_ADDR + 0x2020)
#define rDU4_PZ_MASK			(DU_BASE_ADDR + 0x2024)

// OSD CTRL //
#define rDU4_OSD_ENABLE			(DU_BASE_ADDR + 0x4000)
#define rDU4_OSD_SRC_SEL		(DU_BASE_ADDR + 0x4008)
#define rDU4_OSD_BUS_BW			(DU_BASE_ADDR + 0x400c)
#define rDU4_OSD_FRAME_CNT		(DU_BASE_ADDR + 0x4018)
#define rDU4_OSD_BUS_BW1		(DU_BASE_ADDR + 0x4020)

// OSD FONT //
#define rDU4_FONT_DISP_CTRL		(DU_BASE_ADDR + 0x4100)
#define rDU4_FONT_BASE			(DU_BASE_ADDR + 0x4104)
#define rDU4_FONT_ALPHA			(DU_BASE_ADDR + 0x4108)
#define rDU4_FONT_CFG			(DU_BASE_ADDR + 0x410c)
#define rDU4_FONT_CANVAS		(DU_BASE_ADDR + 0x4110)
#define rDU4_FONT_ORIGIN		(DU_BASE_ADDR + 0x4114)
#define rDU4_FONT_FG0			(DU_BASE_ADDR + 0x4120)
#define rDU4_FONT_FG1			(DU_BASE_ADDR + 0x4124)
#define rDU4_FONT_FG2			(DU_BASE_ADDR + 0x4128)
#define rDU4_FONT_FG3			(DU_BASE_ADDR + 0x412c)
#define rDU4_FONT_FG4			(DU_BASE_ADDR + 0x4130)
#define rDU4_FONT_FG5			(DU_BASE_ADDR + 0x4134)
#define rDU4_FONT_FG6			(DU_BASE_ADDR + 0x4138)
#define rDU4_FONT_FG7			(DU_BASE_ADDR + 0x413c)
#define rDU4_FONT_BD0_0			(DU_BASE_ADDR + 0x4140)
#define rDU4_FONT_BD0_1			(DU_BASE_ADDR + 0x4144)
#define rDU4_FONT_BD1_2			(DU_BASE_ADDR + 0x4148)
#define rDU4_FONT_BD1_3			(DU_BASE_ADDR + 0x414c)
#define rDU4_FONT_BG0			(DU_BASE_ADDR + 0x4150)
#define rDU4_FONT_BG1			(DU_BASE_ADDR + 0x4154)
#define rDU4_FONT_BG2			(DU_BASE_ADDR + 0x4158)
#define rDU4_FONT_BG3			(DU_BASE_ADDR + 0x415c)
#define rDU4_FONT_STATE			(DU_BASE_ADDR + 0x4160)
#define rDU4_FONT_CTRL1			(DU_BASE_ADDR + 0x417c)

// OSD TG //
#define rDU4_TG_CTRL			(DU_BASE_ADDR + 0x4500)
#define rDU4_TG_SIZE			(DU_BASE_ADDR + 0x4500)
#define rDU4_TG_KICK			(DU_BASE_ADDR + 0x4500)
#define rDU4_TG_ACTIVE_X		(DU_BASE_ADDR + 0x4500)
#define rDU4_TG_ACTIVE_Y		(DU_BASE_ADDR + 0x4500)

// OSD RLE AREA0 //
#define rDU4_RLE_AREA0_BASE		(DU_BASE_ADDR + 0x6000)
#define rDU4_RLE_AREA0_BYTE		(DU_BASE_ADDR + 0x6004)
#define rDU4_RLE_AREA0_IMAGE	(DU_BASE_ADDR + 0x6008)
#define rDU4_RLE_AREA0_CFG		(DU_BASE_ADDR + 0x600c)

//OSD RLE0 CANVAS //
#define rDU4_RLE_AREA0_START	(DU_BASE_ADDR + 0x6300)
#define rDU4_RLE_AREA1_START	(DU_BASE_ADDR + 0x6304)
#define rDU4_RLE_AREA2_START	(DU_BASE_ADDR + 0x6308)
#define rDU4_RLE_AREA3_START	(DU_BASE_ADDR + 0x630c)
#define rDU4_RLE_CANVAS_SIZE	(DU_BASE_ADDR + 0x6310)
#define rDU4_RLE_CANVAS_COLOR	(DU_BASE_ADDR + 0x6314)
#define rDU4_RLE_CANVAS_START	(DU_BASE_ADDR + 0x6318)

// OSD R2Y //
#define rDU4_R2Y_YCOEF0			(DU_BASE_ADDR + 0x6380)
#define rDU4_R2Y_YCOEF1			(DU_BASE_ADDR + 0x6384)
#define rDU4_R2Y_YCOEF2			(DU_BASE_ADDR + 0x6388)
#define rDU4_R2Y_YCOEF3			(DU_BASE_ADDR + 0x638c)
#define rDU4_R2Y_UCOEF0			(DU_BASE_ADDR + 0x6390)
#define rDU4_R2Y_UCOEF1			(DU_BASE_ADDR + 0x6394)
#define rDU4_R2Y_UCOEF2			(DU_BASE_ADDR + 0x6398)
#define rDU4_R2Y_UCOEF3			(DU_BASE_ADDR + 0x639c)
#define rDU4_R2Y_VCOEF0			(DU_BASE_ADDR + 0x63a0)
#define rDU4_R2Y_VCOEF1			(DU_BASE_ADDR + 0x63a4)
#define rDU4_R2Y_VCOEF2			(DU_BASE_ADDR + 0x63a8)
#define rDU4_R2Y_VCOEF3			(DU_BASE_ADDR + 0x63ac)

// ATG //
#define rDU4_ATG_TOTAL_X		(DU_BASE_ADDR + 0x3000)
#define rDU4_ATG_TOTAL_Y		(DU_BASE_ADDR + 0x3004)
#define rDU4_ATG_START_X		(DU_BASE_ADDR + 0x3008)
#define rDU4_ATG_START_Y		(DU_BASE_ADDR + 0x300c)
#define rDU4_ATG_END_X			(DU_BASE_ADDR + 0x3010)
#define rDU4_ATG_END_Y			(DU_BASE_ADDR + 0x3014)
#define rDU4_ATG_KICK_X			(DU_BASE_ADDR + 0x3018)
#define rDU4_ATG_CONFIG			(DU_BASE_ADDR + 0x301c)
#define rDU4_ATG_QUEUE_LEVEL	(DU_BASE_ADDR + 0x3020)
#define rDU4_ATG_SYNC_STATE		(DU_BASE_ADDR + 0x3024)
#define rDU4_ATG_IN_ACTIVE_CNT	(DU_BASE_ADDR + 0x3028)
#define rDU4_ATG_IN_BLANK_CNT	(DU_BASE_ADDR + 0x302c)
#define rDU4_ATG_IN_HTOTAL_CNT	(DU_BASE_ADDR + 0x3030)
#define rDU4_ATG_IN_VTOTAL_CNT	(DU_BASE_ADDR + 0x3034)
#define rDU4_ATG_OUT_ACTIVE_CNT	(DU_BASE_ADDR + 0x3038)


extern void du_init (void);

#endif //__DU_H__