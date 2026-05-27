#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h> // 提供原子操作
#include <time.h>

#define GET_MEMBER_OFFSET(type, member)  ((size_t)(&(((type*)0)->member)))

// 使用原子标志来模拟一个自旋锁
// false (0) 表示锁是空闲的
// true  (1) 表示锁被持有
atomic_flag spinlock = ATOMIC_FLAG_INIT;

// 共享资源：一个计数器
long shared_counter = 0;
const long ITERATIONS_PER_THREAD = 1000000; // 每个线程增加一百万次

// 使用自旋锁保护下的递增函数
void increment_with_spinlock() {
    // 尝试获取锁：如果锁是空闲的 (false)，则将其设为 true 并返回 true
    // 如果锁已被持有 (true)，则循环一直“旋转”检查
    while (atomic_flag_test_and_set(&spinlock)) {
        // 自旋等待...
        // 在真实高性能代码中，这里通常会加一个 pause/yield 指令
        // 来提示CPU我们处于自旋状态，减少能耗和总线争用。
        // 例如: _mm_pause(); (x86) 或 sched_yield();
    }

    // --- 临界区开始 ---
    // 成功获取锁，现在可以安全地修改共享资源
    shared_counter++;
    // --- 临界区结束 ---

    // 释放锁：将标志设回 false
    atomic_flag_clear(&spinlock);
}

// 线程函数
void* thread_func(void* arg) {
    long thread_id = (long)arg;
    for (long i = 0; i < ITERATIONS_PER_THREAD; ++i) {
        increment_with_spinlock();
    }
    printf("Thread %ld finished.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    clock_t start, end;
    double cpu_time_used;

    printf("--- Spinlock Demo: Two threads incrementing a counter ---\n");
    printf("Each thread will increment the counter %ld times.\n", ITERATIONS_PER_THREAD);
    printf("Expected final value: %ld\n\n", 2 * ITERATIONS_PER_THREAD);

    start = clock();

    // 创建两个线程
    pthread_create(&t1, NULL, thread_func, (void*)0);
    pthread_create(&t2, NULL, thread_func, (void*)1);

    // 等待两个线程完成
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nFinal counter value: %ld\n", shared_counter);
    printf("Time taken: %.4f seconds\n", cpu_time_used);

    return 0;
}