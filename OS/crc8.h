/*!*****************************************************************************
 * file		crc8.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _CRC8_H
#define _CRC8_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  u8 Crc8_Add(u8 crc, u8 data);
  u8 Crc8_Calculate(const u8 *data, u16 len, u8 crcInit);

#ifdef __cplusplus
}
#endif

#endif
