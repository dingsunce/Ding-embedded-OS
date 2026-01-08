/*!*****************************************************************************
 * file		double_list.h (Double List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef DOUBLE_LIST_H_
#define DOUBLE_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  typedef struct Double_List
  {
    struct Double_List *next;
    struct Double_List *prev;
  } Double_List_t;

  void DoubleList_Init(Double_List_t *list);

  void           DoubleList_Add(Double_List_t *listHeader, Double_List_t *entry);
  void           DoubleList_Push(Double_List_t *listHeader, Double_List_t *entry);
  Double_List_t *DoubleList_Pop(Double_List_t *listHeader);
  bool           DoubleList_IsEmpty(Double_List_t *listHeader);
  u16            DoubleList_Len(const Double_List_t *listHeader);

  void DoubleList_InsertBefore(Double_List_t *list, Double_List_t *entry);
  void DoubleList_InsertAfter(Double_List_t *list, Double_List_t *entry);
  void DoubleList_Remove(Double_List_t *entry);

  void DoubleList_Splice(Double_List_t *list, Double_List_t *append);

#define DOUBLE_LIST_INIT(name) {&(name), &(name)}

#define DOUBLE_LIST(name) static Double_List_t name = DOUBLE_LIST_INIT(name)

#define PUBLIC_DOUBLE_LIST(name) Double_List_t name = DOUBLE_LIST_INIT(name)

#ifdef __cplusplus
}
#endif

#endif
