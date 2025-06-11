#include "bit_array.h"

#include "memory.h"

#define Assert(expr) ((void)0)

#define ROUND_BYTES(len) ((u16)((((len) + 8 - 1) & (~(8 - 1))) / 8))

static const u8 MsbMask[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80};
static const u8 LsbMask[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

BitArray_t *BitArray_New(u16 length)
{
  Assert(length > 0);

#if UNIT_TEST != 1 // cmake compile fail if the following code enabled
  BitArray_t *ba = (BitArray_t *)Mem_Malloc(sizeof(BitArray_t));
  if (ba == NULL)
  {
    return NULL;
  }

  ba->Bytes = (u8 *)Mem_Malloc(ROUND_BYTES(length));
  if (ba->Bytes == NULL)
  {
    Mem_Free(ba);
    return NULL;
  }
  ba->Length = length;

  return ba;
#else
  return NULL;
#endif
}

void BitArray_Free(BitArray_t *ba)
{
  Assert(ba && ba->Bytes);

  if (ba != NULL)
  {
    if (ba->Bytes != NULL)
    {
      Mem_Free(ba->Bytes);
    }
    Mem_Free(ba);
  }
}

u16 BitArray_Length(const BitArray_t *ba)
{
  Assert(ba);

  return ba->Length;
}

u16 BitArray_CountOf0(const BitArray_t *ba)
{
  Assert(ba);

  return ba->Length - BitArray_CountOf1(ba);
}

// Get number of '1' in the array
u16 BitArray_CountOf1(const BitArray_t *ba)
{
  Assert(ba);

  static const u8 countOf1[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

  u16 count = 0;
  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    u8 byte = ba->Bytes[i];
    count += countOf1[byte & 0xF] + countOf1[byte >> 4];
  }

  return count;
}

bool BitArray_IsAll0(const BitArray_t *ba)
{
  Assert(ba);

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    if (ba->Bytes[i] != 0)
    {
      return false;
    }
  }

  return true;
}

bool BitArray_IsAll1(const BitArray_t *ba)
{
  Assert(ba);

  for (u16 i = 0; i < ROUND_BYTES(ba->Length) - 1; i++)
  {
    if (ba->Bytes[i] != 0xFF)
    {
      return false;
    }
  }

  if ((ba->Bytes[ROUND_BYTES(ba->Length) - 1] & LsbMask[ba->Length / 8]) != LsbMask[ba->Length / 8])
  {
    return false;
  }

  return true;
}

bool BitArray_IsAll0ByMask(const BitArray_t *ba, const BitArray_t *mask)
{
  Assert(ba);
  Assert(mask);
  Assert(ba->Length == mask->Length);
  Assert(!BitArray_IsAll0(mask));

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    if ((ba->Bytes[i] & mask->Bytes[i]) != 0)
    {
      return false;
    }
  }

  return true;
}

bool BitArray_IsAll1ByMask(const BitArray_t *ba, const BitArray_t *mask)
{
  Assert(ba);
  Assert(mask);
  Assert(ba->Length == mask->Length);
  Assert(!BitArray_IsAll0(mask));

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    if ((ba->Bytes[i] & mask->Bytes[i]) != mask->Bytes[i])
    {
      return false;
    }
  }

  return true;
}

bool BitArray_GetBit(const BitArray_t *ba, u16 index)
{
  Assert(ba);
  Assert(index < ba->Length);

  return ((ba->Bytes[index / 8] >> (index % 8)) & 1);
}

void BitArray_WriteBit(const BitArray_t *ba, u16 index, bool value)
{
  Assert(ba);
  Assert(index < ba->Length);

  if (value)
    ba->Bytes[index / 8] |= 1 << (index % 8);
  else
    ba->Bytes[index / 8] &= ~(1 << (index % 8));
}

void BitArray_SetBit(const BitArray_t *ba, u16 index)
{
  Assert(ba);
  Assert(index < ba->Length);

  ba->Bytes[index / 8] |= 1 << (index % 8);
}

void BitArray_ClearBit(const BitArray_t *ba, u16 index)
{
  Assert(ba);
  Assert(index < ba->Length);

  ba->Bytes[index / 8] &= ~(1 << (index % 8));
}

void BitArray_NotBit(const BitArray_t *ba, u16 index)
{
  Assert(ba);
  Assert(index < ba->Length);

  ba->Bytes[index / 8] ^= 1 << (index % 8);
}

void BitArray_SetByRange(const BitArray_t *ba, u16 start, u16 end)
{
  Assert(ba);
  Assert(start > 0 && end < ba->Length);
  Assert(start <= end);

  if (start / 8 < end / 8)
  {
    ba->Bytes[start / 8] |= MsbMask[start % 8];
    for (u16 i = start / 8 + 1; i < end / 8; i++)
    {
      ba->Bytes[i] = 0xFF;
    }
    ba->Bytes[end / 8] |= LsbMask[end % 8];
  }
  else
    ba->Bytes[start / 8] |= (MsbMask[start % 8] & LsbMask[end % 8]);
}

void BitArray_ClearByRange(const BitArray_t *ba, u16 start, u16 end)
{
  Assert(ba);
  Assert(start > 0 && end < ba->Length);
  Assert(start <= end);

  if (start / 8 < end / 8)
  {
    ba->Bytes[start / 8] &= ~MsbMask[start % 8];
    for (u16 i = start / 8 + 1; i < end / 8; i++)
    {
      ba->Bytes[i] = 0;
    }
    ba->Bytes[end / 8] &= ~LsbMask[end % 8];
  }
  else
    ba->Bytes[start / 8] &= ~(MsbMask[start % 8] & LsbMask[end % 8]);
}

void BitArray_NotByRange(const BitArray_t *ba, u16 start, u16 end)
{
  Assert(ba);
  Assert(start > 0 && end < ba->Length);
  Assert(start <= end);

  if (start / 8 < end / 8)
  {
    ba->Bytes[start / 8] ^= MsbMask[start % 8];
    for (u16 i = start / 8 + 1; i < end / 8; i++)
    {
      ba->Bytes[i] ^= 0xFF;
    }
    ba->Bytes[end / 8] ^= LsbMask[end % 8];
  }
  else
    ba->Bytes[start / 8] ^= (MsbMask[start % 8] & LsbMask[end % 8]);
}

static void ClearSpareBits(const BitArray_t *ba)
{
  Assert(ba);

  u8 spare = ba->Length % 8;
  if (spare != 0)
  {
    ba->Bytes[ROUND_BYTES(ba->Length) - 1] &= LsbMask[spare];
  }
}

void BitArray_SetByMask(const BitArray_t *ba, const BitArray_t *mask)
{
  Assert(ba);
  Assert(mask);
  Assert(ba->Length == mask->Length);

  if (BitArray_IsAll0(mask))
  {
    return;
  }

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    ba->Bytes[i] |= mask->Bytes[i];
  }

  ClearSpareBits(ba);
}

void BitArray_ClearByMask(const BitArray_t *ba, const BitArray_t *mask)
{
  Assert(ba);
  Assert(mask);
  Assert(ba->Length == mask->Length);

  if (BitArray_IsAll0(mask))
  {
    return;
  }

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    ba->Bytes[i] &= ~mask->Bytes[i];
  }

  ClearSpareBits(ba);
}

void BitArray_NotByMask(const BitArray_t *ba, const BitArray_t *mask)
{
  Assert(ba);
  Assert(mask);
  Assert(ba->Length == mask->Length);

  if (BitArray_IsAll0(mask))
  {
    return;
  }

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    ba->Bytes[i] ^= mask->Bytes[i];
  }

  ClearSpareBits(ba);
}

void BitArray_SetAll(const BitArray_t *ba)
{
  Assert(ba);

  memset(ba->Bytes, 0xFF, ROUND_BYTES(ba->Length));

  ClearSpareBits(ba);
}

void BitArray_ClearAll(const BitArray_t *ba)
{
  Assert(ba);

  memset(ba->Bytes, 0, ROUND_BYTES(ba->Length));
}

void BitArray_NotAll(const BitArray_t *ba)
{
  Assert(ba);

  for (u16 i = 0; i < ROUND_BYTES(ba->Length); i++)
  {
    ba->Bytes[i] ^= 0xFF;
  }

  ClearSpareBits(ba);
}

void BitArray_Copy(BitArray_t *dest, const BitArray_t *src)
{
  Assert(dest);
  Assert(src);
  Assert(dest->Length == src->Length);

  memcpy(dest->Bytes, src->Bytes, ROUND_BYTES(src->Length));
}

// Create and copy
BitArray_t *BitArray_Duplicate(const BitArray_t *src)
{
  Assert(src);

  BitArray_t *ba = BitArray_New(src->Length);
  if (ba != NULL)
  {
    ba->Length = src->Length;
    BitArray_Copy(ba, src);
  }

  return ba;
}

void BitArray_And(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2)
{
  Assert(dest);
  Assert(src1);
  Assert(src2);
  Assert(dest->Length == src1->Length);
  Assert(dest->Length == src2->Length);

  for (u16 i = 0; i < ROUND_BYTES(dest->Length); i++)
  {
    dest->Bytes[i] = src1->Bytes[i] & src2->Bytes[i];
  }

  ClearSpareBits(dest);
}

void BitArray_Or(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2)
{
  Assert(dest);
  Assert(src1);
  Assert(src2);
  Assert(dest->Length == src1->Length);
  Assert(dest->Length == src2->Length);

  for (u16 i = 0; i < ROUND_BYTES(dest->Length); i++)
  {
    dest->Bytes[i] = src1->Bytes[i] | src2->Bytes[i];
  }

  ClearSpareBits(dest);
}

void BitArray_Xor(const BitArray_t *dest, const BitArray_t *src1, const BitArray_t *src2)
{
  Assert(dest);
  Assert(src1);
  Assert(src2);
  Assert(dest->Length == src1->Length);
  Assert(dest->Length == src2->Length);

  for (u16 i = 0; i < ROUND_BYTES(dest->Length); i++)
  {
    dest->Bytes[i] = src1->Bytes[i] ^ src2->Bytes[i];
  }

  ClearSpareBits(dest);
}