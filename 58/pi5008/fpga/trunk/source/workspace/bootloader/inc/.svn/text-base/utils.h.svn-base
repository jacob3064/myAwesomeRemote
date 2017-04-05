/**
 * \file
 *
 * \brief	utils
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

#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/***************************************************************************************************************************************************************
 * param value	:	Number to be aligned by n
 * param n		:	Number of alignment
 * return		:	Result

 * brief
	Calculates the number which is aligned by n and is not less than value.
	n must be a power-of-2 number.
***************************************************************************************************************************************************************/
#define utilAlign2( value, n )						( ( value )+( ( ( ~( value ) )+1 )&( ( n )-1 ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Number to be checked
 * param n		:	Number of alignment
 * return		:	True if value is aligned by n, false otherwise

 * brief
	Check whether val is aligned by n.
	n must be a power-of-2 number.
***************************************************************************************************************************************************************/
#define utilIsAligned2( value, n )					( !( ( value )&( ( n )-1 ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Number to be truncated
 * param n		:	Number of truncation
 * return		:	Result

 * brief
	Calculates truncated integer value.
	n must be a power-of-2 number.
***************************************************************************************************************************************************************/
#define utilTruncate2( value, n )					( ( value )&~( ( n )-1 ) )

/***************************************************************************************************************************************************************
 * param value	:	Number to be circulated
 * param i		:	Number to be added
 * param n		:	Boundary
 * return		:	Result

 * brief
	Calculates the number which has displacement from the val by i.
	It has a boundary at the corner of which the displacement wraps around.
	n must be a power-of-2 number.
***************************************************************************************************************************************************************/
#define utilRCirculate2( value, i, n )				( ( ( value )+i )&( ( n )-1 ) )

/***************************************************************************************************************************************************************
 * param n	:	Exponent value
 * return	:	Result

 * brief
	Calculates power of 2 by n.
***************************************************************************************************************************************************************/
#define utilPower2( n )								( 1ULL<<( n ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to be checked
 * return		:	True if value is power of 2, false otherwise.

 * brief
	Checks whether value is power of 2. It returns if the value is
	power of 2 and false otherwise.
***************************************************************************************************************************************************************/
#define utilIsPower2( value )						( !( ( value )&( ( value )-1 ) ) )

/***************************************************************************************************************************************************************
 * param n	:	Number of bits
 * return	:	Result

 * brief
	Generates all 1's bitmask of n-bit size.
***************************************************************************************************************************************************************/
#define utilBitMask( n )							( utilPower2( n )-1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to check
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Checks whether subfield of value located by pos & size
	contains non-zero value. it returns non-zero value if so
	and 0 otherwise. Bit position 0 starts from the lsb.
***************************************************************************************************************************************************************/
#define utilTestBits( value, pos, size )			( ( value )& ( utilBitMask( size )<<( pos ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Clears subfield of value located by pos & size to 0.
	Bit position 0 starts from the lsb.
***************************************************************************************************************************************************************/
#define utilClearBits( value, pos, size )			( ( value )&~( utilBitMask( size )<<( pos ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Sets subfield of value located by pos & size to all 1's.
	Bit position 0 starts from the lsb.
***************************************************************************************************************************************************************/
#define utilSetBits( value, pos, size )				( ( value )|( utilBitMask( size )<<( pos ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Inverts subfield of value located by pos & size.
	Bit position 0 starts from the lsb.
***************************************************************************************************************************************************************/
#define utilInvertBits( value, pos, size )			( ( value )^( utilBitMask( size )<<( pos ) ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Gets the value of subfield of value located by pos & size and
	returns it. Bit position 0 starts from the lsb.
***************************************************************************************************************************************************************/
#define utilGetBits( value, pos, size )				( ( ( value )>>( pos ) )&utilBitMask( size ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * return		:	Result

 * brief
	Generates a bit pattern which has the subfield located
	by pos & size and value of the subfield is equal to value.
***************************************************************************************************************************************************************/
#define utilPackBits( value, pos, size )			( ( ( value )&utilBitMask( size ) )<<( pos ) )

/***************************************************************************************************************************************************************
 * param dst	:	Value to operate on
 * param pos	:	Bit position
 * param size	:	Bit size
 * param src	:	Value to replace
 * return		:	Result

 * brief
	Substitutes the subfield of dst located by pos & siz by src
	and returns its value.
***************************************************************************************************************************************************************/
#define utilPutBits( dst, pos, size, src )			( utilClearBits( dst, pos, size )|utilPackBits( src, pos, size ) )

/***************************************************************************************************************************************************************
 * param value	:	Value to check
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Checks whether the bit of value at pos is 1 or 0.
***************************************************************************************************************************************************************/
#define utilTestBit( value, pos )					utilTestBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Clears the bit of value at pos to 0.
***************************************************************************************************************************************************************/
#define utilClearBit( value, pos )					utilClearBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Sets the bit of value at pos to 1.
***************************************************************************************************************************************************************/
#define utilSetBit( value, pos )					utilSetBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Inverts the bit of value at pos to 1.
***************************************************************************************************************************************************************/
#define utilInvertBit( value, pos )					 utilInvertBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Gets the value of the bit of value at pos and returns it.
****************************************************************************************************************************************************************/
#define utilGetBit( value, pos )					utilGetBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param value	:	Value to operate on
 * param pos	:	Bit position
 * return		:	Result

 * brief
	Generates a bit pattern which has the bit at pos and
	its value is equal to value.
***************************************************************************************************************************************************************/
#define utilPackBit( value, pos )					utilPackBits( value, pos, 1 )

/***************************************************************************************************************************************************************
 * param dst	:	Value to operate on
 * param pos	:	Bit position
 * param src	:	Value to replace
 * return		:	Result

 * brief
	Substitutes the bit of dst at pos by src and returns its value.
***************************************************************************************************************************************************************/
#define utilPutBit( dst, pos, src )					utilPutBits( dst, pos, 1, src )

/***************************************************************************************************************************************************************
 * param value	:	Value to round
 * return		:	Result

 * brief
	Rounds value to the nearest.
***************************************************************************************************************************************************************/
#define utilRound( value )							( ( int )( ( value )+0.5f ) )

/***************************************************************************************************************************************************************
 * param a	:	Number
 * return	:	Result

 * brief
	Calculates the absolute value of the given number.
	It is mainly used to represent fixed-point constants by floating-point value.
***************************************************************************************************************************************************************/
#define utilAbs( a )								( ( ( a )>=0 ) ? ( a ) : -( a ) )

/***************************************************************************************************************************************************************
 * param a	:	1st value
 * param b	:	2nd value
 * return	:	Result

 * brief
	Compares the 2 values and then returns the bigger or equal one.
***************************************************************************************************************************************************************/
#define utilMax( a, b )								( ( ( a )>( b ) ) ? ( a ) : ( b ) )

/***************************************************************************************************************************************************************
 * param a	:	1st value
 * param b	:	2nd value
 * return	:	Result

 * brief
	Compares the 2 values and then returns the smaller or equal one.
***************************************************************************************************************************************************************/
#define utilMin( a, b )								( ( ( a )<( b ) ) ? ( a ) : ( b ) )

#ifdef __cplusplus
}
#endif

#endif // __UTILS_H__