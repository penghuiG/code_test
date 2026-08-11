#ifndef _TASK_LIST_H_
#define _TASK_LIST_H_

#include "task.h"

typedef struct _task_list_t
{
    task_t *task;
    struct _task_list_t *next;
    
}task_list_t;

int task_add(task_t * task);
void* work_handle(void *arg);
void* work_handle_1(void *arg);
#endif