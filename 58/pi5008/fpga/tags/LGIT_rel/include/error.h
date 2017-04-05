/**
 * \file
 *
 * \brief	Error code
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

#ifndef __ERROR_H__
#define __ERROR_H__

enum tagERROR_E
{
	eSUCCESS = 0,
	eERROR_INVALIED_ARGUMENT,											/* Invalid argument */
	eERROR_MEM_ALLOC,													/* Out of memory */

	eERROR_SVM_NOT_INITIALIZE = 100,									/* Not Initialize */
	eERROR_SVM_VIEWMODE_COUNT_ZERO,										/* Viewmode count is 0 */
	eERROR_SVM_VIEWMODE_NOT_SELECTED,									/* Not selected viewmode */

	eERROR_VPU_NOT_INITIALIZE = 200,									/* Not Initialize */
	eERROR_VPU_INVALIED_ARGUMENT_TOO_MAANY_LIMIT_FEATURE_COUNT,			/* Invalid argument */
	eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_1ST_FRAME,					/* Too many feature count (1st frame)*/
	eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_2ND_FRAME,					/* Too many feature count (2nd frame)*/
	eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_BOTH_FRAME,				/* Too many feature count (1st frame and 2nd frame)*/

	eERROR_SYS_DMA_INVALID_CHANNEL = 300,								/* Invalid DMA channel */
	eERROR_SYS_DMA_CHANNEL_IN_USE,										/* DMA channel is already in use */
	eERROR_SYS_DMA_ABORT,												/* DMA abort */
	eERROR_SYS_DMA_ERROR,												/* DMA error */
};

#endif // __ERROR_H__