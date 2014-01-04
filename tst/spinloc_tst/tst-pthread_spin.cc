/* Copyright (C) 2002-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2002.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

// #include <pthread.h>
#include <cstdio>
#include <iostream>
#include <pthread.h>

#define CUTE_NUM 0x7fba2
#define LATE_NUM 0x9000

pthread_spinlock_t sl;

static void* compute_one(void* arg)
{
    int m = *((int*)arg);
    int n = *((int*)arg + 1);
    // std::cout << "m: " << m << "\tn: " << n << std::endl;
    pthread_spin_lock(&sl);
    unsigned int mydata = CUTE_NUM;
    unsigned int tmp = 0, result = 0;
    for(int i = m; i < n; ++i) {
        tmp = i;
        result += (tmp & (--mydata));

// #if 0
        if(0 == (i % LATE_NUM)) {
            pthread_spin_unlock(&sl);
            sleep(1);
            pthread_spin_lock(&sl);
        }
// #endif
    }
    pthread_spin_unlock(&sl);

    std::cout << "Thread: " << pthread_self() << "\tresutl: " << result << std::endl;

}

static void* compute_try(void* arg)
{
    int cnt = 100;
    do {
        while(0 != pthread_spin_trylock(&sl)) ;
        --cnt;
        pthread_spin_unlock(&sl);

        if(0 == (cnt % 12)) {
            pthread_spin_unlock(&sl);
            sleep(1);
        }
    } while(0 < cnt);

    std::cout << "Thread: " << pthread_self() << "\ttake spinlock 100 times" << std::endl;

}

static int
do_test (void)
{
    int ary1[2] = {1, 0x7ffff};
    int ary2[2] = {3, 0x3ffff};
    pthread_spin_init(&sl, PTHREAD_PROCESS_PRIVATE);
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, compute_one, (void*)&ary1);
    pthread_create(&t2, NULL, compute_one, (void*)&ary2);
    pthread_create(&t3, NULL, &compute_try, NULL);

    sleep(1);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
}

int main()
{
    do_test();
    return 0;
}
// #define TEST_FUNCTION do_test ()
// #include "../test-skeleton.c"
