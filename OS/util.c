#include "util.h"

bool Util_CompareBitArray(const bool *pArray1, const bool *pArray2, u16 length)
{
  if (pArray1 == NULL || pArray2 == NULL)
  {
    return false;
  }

  for (u16 i = 0; i < length; i++)
  {
    if (pArray1[i] != pArray2[i])
    {
      return false;
    }
  }

  return true;
}

bool Util_CompareByteArray(const u8 *pArray1, const u8 *pArray2, u16 length)
{
  if (pArray1 == NULL || pArray2 == NULL)
  {
    return false;
  }

  for (u16 i = 0; i < length; i++)
  {
    if (pArray1[i] != pArray2[i])
    {
      return false;
    }
  }

  return true;
}

bool Util_CompareWordArray(const u16 *pArray1, const u16 *pArray2, u16 length)
{
  if (pArray1 == NULL || pArray2 == NULL)
  {
    return false;
  }

  for (u16 i = 0; i < length; i++)
  {
    if (pArray1[i] != pArray2[i])
    {
      return false;
    }
  }

  return true;
}