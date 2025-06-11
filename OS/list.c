/*!*****************************************************************************
 * file		list.c (another implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/
#include "list.h"

typedef struct ListContent
{
  struct ListContent *next;
} ListContent_t;

//-----------------------------------------------------------------------------------------------------------
void List_Init(List_t list)
{
  *list = NULL;
}
//-----------------------------------------------------------------------------------------------------------
void *List_Head(List_t list)
{
  return *list;
}
//-----------------------------------------------------------------------------------------------------------
void List_Copy(List_t dest, List_t src)
{
  *dest = *src;
}
//-----------------------------------------------------------------------------------------------------------
void *List_Tail(List_t list)
{
  ListContent_t *l;

  if (*list == NULL)
  {
    return NULL;
  }

  for (l = *list; l->next != NULL; l = l->next)
    ;

  return l;
}
//-----------------------------------------------------------------------------------------------------------
void List_Add(List_t list, void *item)
{
  // Add an item to the end of the list.
  ListContent_t *l;

  if (item == NULL)
    return;

  ((ListContent_t *)item)->next = NULL;

  l = List_Tail(list);

  if (l == NULL)
  {
    *list = item;
  }
  else
  {
    l->next = item;
  }
}
//-----------------------------------------------------------------------------------------------------------
void List_Push(List_t list, void *item)
{
  if (item == NULL)
    return;

  ((ListContent_t *)item)->next = *list;
  *list = item;
}
//-----------------------------------------------------------------------------------------------------------
void *List_Chop(List_t list)
{
  // Remove the last object on the list.
  ListContent_t *l, *r;

  if (*list == NULL)
    return NULL;

  if (((ListContent_t *)*list)->next == NULL)
  {
    l = *list;
    *list = NULL;
    return l;
  }

  for (l = *list; l->next->next != NULL; l = l->next)
    ;

  r = l->next;
  l->next = NULL;

  return r;
}
//-----------------------------------------------------------------------------------------------------------
void *List_Pop(List_t list)
{
  // Remove the first object on a list.
  ListContent_t *l;
  l = *list;

  if (*list != NULL)
    *list = ((ListContent_t *)*list)->next;

  return l;
}
//-----------------------------------------------------------------------------------------------------------
void List_Remove(List_t list, void *item)
{
  ListContent_t *l, *r;

  if (*list == NULL)
    return;

  r = NULL;
  for (l = *list; l != NULL; l = l->next)
  {
    if (l == item)
    {
      if (r == NULL)
        *list = l->next;
      else
        r->next = l->next;

      l->next = NULL;
      return;
    }

    r = l;
  }
}
//-----------------------------------------------------------------------------------------------------------
u32 List_Length(List_t list)
{
  ListContent_t *l;
  u32            n = 0;

  for (l = *list; l != NULL; l = l->next)
    ++n;

  return n;
}
//-----------------------------------------------------------------------------------------------------------
bool List_IsEmpty(List_t list)
{
  if (List_Head(list) != NULL)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void List_Insert(List_t list, void *prevItem, void *newItem)
{
  if (prevItem == NULL)
  {
    List_Push(list, newItem);
  }
  else
  {

    ((ListContent_t *)newItem)->next = ((ListContent_t *)prevItem)->next;
    ((ListContent_t *)prevItem)->next = newItem;
  }
}
//-----------------------------------------------------------------------------------------------------------
void *List_ItemNext(void *item)
{
  return item == NULL ? NULL : ((ListContent_t *)item)->next;
}
