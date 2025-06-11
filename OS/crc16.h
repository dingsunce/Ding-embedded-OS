/*!*****************************************************************************
 * file		crc16.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _CRC16_H
#define _CRC16_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  u16 Crc16_Add(u16 crcInit, u8 data);
  u16 Crc16_Calculate(const u8 *data, u16 len, u16 crcInit);

#ifdef __cplusplus
}
#endif

#endif
