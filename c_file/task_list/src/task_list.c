#include "task_list.h"

static task_list_t *task_list = NULL;
pthread_mutex_t counter_mutex;
pthread_cond_t task_cond;

int task_add(task_t * task)
{
    if(!task) return -1;
    pthread_mutex_lock(&counter_mutex);
    task_list_t *node = (task_list_t*)malloc(sizeof(task_list_t));
    node->task = task;
    static task_list_t *q = NULL;
    q = task_list;
    if(!task_list)
    {
        task_list = node;
        task_list->next = NULL;
    }
    else
    {
        while(q->next != NULL)
        {
            q = q->next;
        }
        q->next = node;
        q = q->next;
        q->next = NULL;
    }
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&counter_mutex);
    
    return 0;
}

void* work_handle(void *arg)
{

    while(1)
    {
        task_list_t *p = NULL;
        pthread_mutex_lock(&counter_mutex);
        while(task_list == NULL)
        {
            pthread_cond_wait(&task_cond,&counter_mutex);
        }   
        p = task_list;
        task_list = task_list->next;

        pthread_mutex_unlock(&counter_mutex);

        if(p)
        {
            if(p->task->check(p->task->data) == 0)
            {
                p->task->work(p->task->data);
            }
            free(p);
        }
        
        sleep(1);
    }
}