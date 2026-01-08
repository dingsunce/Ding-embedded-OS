/*!*****************************************************************************
 * file		double_list.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "double_list.h"
//-----------------------------------------------------------------------------------------------------------
void DoubleList_Init(Double_List_t *list)
{
  list->next = list->prev = list;
}
//-----------------------------------------------------------------------------------------------------------
static void Add(Double_List_t *entry, Double_List_t *prev, Double_List_t *next)
{
  prev->next = entry;
  entry->prev = prev;

  entry->next = next;
  next->prev = entry;
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_Push(Double_List_t *listHeader, Double_List_t *entry)
{
  // insert entry at beginning of a list header
  Add(entry, listHeader, listHeader->next);
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_InsertAfter(Double_List_t *list, Double_List_t *entry)
{
  // insert a entry after a list
  Add(entry, list, list->next);
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_Add(Double_List_t *listHeader, Double_List_t *entry)
{
  // insert entry at end of a list header
  Add(entry, listHeader->prev, listHeader);
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_InsertBefore(Double_List_t *list, Double_List_t *entry)
{
  // insert a entry before a list
  Add(entry, list->prev, list);
}
//-----------------------------------------------------------------------------------------------------------
static void Remove(Double_List_t *prev, Double_List_t *next)
{
  prev->next = next;
  next->prev = prev;
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_Remove(Double_List_t *entry)
{
  Remove(entry->prev, entry->next);
}
//-----------------------------------------------------------------------------------------------------------
Double_List_t *DoubleList_Pop(Double_List_t *listHeader)
{
  // Remove the first object on a list.
  Double_List_t *got = listHeader->next;
  if (got == listHeader)
    return NULL;

  DoubleList_Remove(got);
  return got;
}
//-----------------------------------------------------------------------------------------------------------
bool DoubleList_IsEmpty(Double_List_t *listHeader)
{
  return listHeader->next == listHeader;
}
//-----------------------------------------------------------------------------------------------------------
void DoubleList_Splice(Double_List_t *list, Double_List_t *append)
{
  // join two lists
  if (list == list->next) // empty list
    return;

  if (append == append->next) // empty append list
    return;

  Double_List_t *lastOfList = list->prev;
  Double_List_t *firstOfAppend = append->next;
  Double_List_t *lastOfAppend = append->prev;

  lastOfList->next = firstOfAppend;
  firstOfAppend->prev = lastOfList;

  lastOfAppend->next = list;
  list->prev = lastOfAppend;
}
//-----------------------------------------------------------------------------------------------------------
u16 DoubleList_Len(const Double_List_t *listHeader)
{
  unsigned int         len = 0;
  const Double_List_t *p = listHeader;
  while (p->next != listHeader)
  {
    p = p->next;
    len++;
  }

  return len;
}