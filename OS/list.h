/*!*****************************************************************************
 * file		list.h (another implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef void **List_t;

#define LIST(name)                                                                                           \
  static void  *name##List = NULL;                                                                           \
  static List_t name = (List_t) & name##List

#define LIST_STRUCT(name)                                                                                    \
  void  *name##List;                                                                                         \
  List_t name

#define LIST_STRUCT_INIT(struct_ptr, name)                                                                   \
  (struct_ptr)->name##List = NULL;                                                                           \
  (struct_ptr)->name = (List_t) & ((struct_ptr)->name##List);                                                \
  List_Init((struct_ptr)->name)

#define LIST_HEADER void *next

  void  List_Init(List_t list);
  void *List_Head(List_t list);
  void *List_Tail(List_t list);
  void *List_Pop(List_t list);
  void  List_Push(List_t list, void *item);
  void *List_Chop(List_t list);
  void  List_Add(List_t list, void *item);
  void  List_Remove(List_t list, void *item);
  bool  List_IsEmpty(List_t list);
  u32   List_Length(List_t list);
  void  List_Copy(List_t dest, List_t src);
  void  List_Insert(List_t list, void *prevItem, void *newItem);
  void *List_ItemNext(void *item);

#ifdef __cplusplus
}
#endif

#endif
