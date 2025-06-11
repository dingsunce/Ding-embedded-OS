#ifndef _UTIL_H
#define _UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  extern bool Util_CompareBitArray(const bool *pArray1, const bool *pArray2, u16 length);
  extern bool Util_CompareByteArray(const u8 *pArray1, const u8 *pArray2, u16 length);
  extern bool Util_CompareWordArray(const u16 *pArray1, const u16 *pArray2, u16 length);

#ifdef __cplusplus
}
#endif

#endif