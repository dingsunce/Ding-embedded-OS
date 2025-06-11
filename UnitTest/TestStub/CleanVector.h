/*!*****************************************************************************
 * $Author: sunce.ding
 * Copyright (c) 2015 B.E.G.
 *******************************************************************************/
#ifndef CLEAR_VECTOR_H
#define CLEAR_VECTOR_H

#include <vector>

template <typename T> void CleanVector(std::vector<T>& vector)
{
  vector.clear();
  vector.shrink_to_fit();
}

#endif
