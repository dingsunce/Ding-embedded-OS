#ifndef _BIT_ARRAY_H
#define _BIT_ARRAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef struct
  {
    u16 Length;
    u8 *Bytes;
  } BitArray_t;

  extern BitArray_t *BitArray_New(u16 length);
  extern void        BitArray_Free(BitArray_t *ba);

  extern u16 BitArray_Length(const BitArray_t *ba);

  extern u16  BitArray_CountOf0(const BitArray_t *ba);
  extern u16  BitArray_CountOf1(const BitArray_t *ba);
  extern bool BitArray_IsAll0(const BitArray_t *ba);
  extern bool BitArray_IsAll1(const BitArray_t *ba);

  extern bool BitArray_IsAll0ByMask(const BitArray_t *ba, const BitArray_t *mask);
  extern bool BitArray_IsAll1ByMask(const BitArray_t *ba, const BitArray_t *mask);

  extern bool BitArray_GetBit(const BitArray_t *ba, u16 index);

  extern void BitArray_WriteBit(const BitArray_t *ba, u16 index, bool value);
  extern void BitArray_SetBit(const BitArray_t *ba, u16 index);
  extern void BitArray_ClearBit(const BitArray_t *ba, u16 index);
  extern void BitArray_NotBit(const BitArray_t *ba, u16 index);

  extern void BitArray_SetByRange(const BitArray_t *ba, u16 start, u16 end);
  extern void BitArray_ClearByRange(const BitArray_t *ba, u16 start, u16 end);
  extern void BitArray_NotByRange(const BitArray_t *ba, u16 start, u16 end);

  extern void BitArray_SetByMask(const BitArray_t *ba, const BitArray_t *mask);
  extern void BitArray_ClearByMask(const BitArray_t *ba, const BitArray_t *mask);
  extern void BitArray_NotMask(const BitArray_t *ba, const BitArray_t *mask);

  extern void BitArray_SetAll(const BitArray_t *ba);
  extern void BitArray_ClearAll(const BitArray_t *ba);
  extern void BitArray_NotAll(const BitArray_t *ba);

  extern void        BitArray_Copy(BitArray_t *dest, const BitArray_t *src);
  extern BitArray_t *BitArray_Duplicate(const BitArray_t *src); // Create and copy
  extern void        BitArray_And(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2);
  extern void        BitArray_Or(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2);
  extern void        BitArray_Xor(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2);

#ifdef __cplusplus
}
#endif

#endif
