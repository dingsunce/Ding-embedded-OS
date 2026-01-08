/*!*****************************************************************************
 * file		d_list.h (another implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef d_list_H_
#define d_list_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef void **d_list_t;

#define LIST(name)                                                                                 \
  static void    *name##List = NULL;                                                               \
  static d_list_t name = (d_list_t) & name##List

#define LIST_STRUCT(name)                                                                          \
  void    *name##List;                                                                             \
  d_list_t name

#define LIST_STRUCT_INIT(struct_ptr, name)                                                         \
  (struct_ptr)->name##List = NULL;                                                                 \
  (struct_ptr)->name = (d_list_t) & ((struct_ptr)->name##List);                                    \
  List_Init((struct_ptr)->name)

#define LIST_HEADER void *next

  void  List_Init(d_list_t list);
  void *List_Head(d_list_t list);
  void *List_Tail(d_list_t list);
  void *List_Pop(d_list_t list);
  void  List_Push(d_list_t list, void *item);
  void *List_Chop(d_list_t list);
  void  List_Add(d_list_t list, void *item);
  void  List_Remove(d_list_t list, void *item);
  bool  List_IsEmpty(d_list_t list);
  u32   List_Length(d_list_t list);
  void  List_Copy(d_list_t dest, d_list_t src);
  void  List_Insert(d_list_t list, void *prevItem, void *newItem);
  void *List_ItemNext(void *item);

#ifdef __cplusplus
}
#endif

#endif
