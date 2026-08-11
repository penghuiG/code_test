#include "task_list.h"
#include <pthread.h>

int task_check_i()
{
    return 0;
}
int task_work_i(void *data)
{
    int x = *(int*)data;
    for(int i=0;i<1;++i)
    {
        printf("this is task %d\n",x);
        sleep(1);
    }
    return 0;
}
int main()
{
    pthread_t task_thread ;
    int a = 10086;
    int b = 10010;

    pthread_create(&task_thread,NULL,work_handle,NULL);
    
    task_t *task = task_creat("first task",task_work_i,task_check_i,(void*)&a);
    task_t *task_2 = task_creat("second task",task_work_i,task_check_i,(void*)&b);
    if(task_add(task) == 0)
    {
    }
    if(task_add(task_2) == 0)
    {
    }
    
    sleep(10);
    
    for(int i = 0;i<5;++i)
    {
        int t[5] = {0,1,2,3,4};
        // printf("%d\n",t);
        task_t *task_x = task_creat("task i",task_work_i,task_check_i,(void*)&t[i]);
        if(task_add(task_x) == 0)
        {
        }
        sleep(1);
    }
    while(1)
    {
        sleep(1);
    }
}