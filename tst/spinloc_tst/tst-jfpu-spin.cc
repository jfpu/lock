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
#include <thread>

#include "mutex.h"

#define CUTE_NUM 0x7fba2
#define LATE_NUM 0x9000

jfpu::spinlock sl;

void compute_one(unsigned int m, unsigned int n)
{
    sl.lock();
    unsigned int mydata = CUTE_NUM;
    unsigned int tmp = 0, result = 0;
    for(int i = m; i < n; ++i) {
        tmp = i;
        result += (tmp & (--mydata));

// #if 0
        if(0 == (i % LATE_NUM)) {
            sl.unlock();
            sleep(1);
            sl.lock();
        }
// #endif
    }
    sl.unlock();

    std::cout << "Thread: " << std::this_thread::get_id() << "\tresutl: " << result << std::endl;

}

void compute_try()
{
    int cnt = 100;
    do {
        while(0 != sl.trylock()) ;
        --cnt;
        sl.unlock();

        if(0 == (cnt % 12)) {
            sl.unlock();
            sleep(1);
        }
    } while(0 < cnt);

    std::cout << "Thread: " << std::this_thread::get_id() << "\ttake spinlock 100 times" << std::endl;

}

static int
do_test (void)
{
    std::thread th1(compute_one, 1, 0x7ffff);
    std::thread th2(compute_one, 1023, 0x3ffff);
    std::thread th3(compute_try);

    sleep(1);
    th1.join();
    th2.join();
    th3.join();
}

int main()
{
    do_test();
    return 0;
}
// #define TEST_FUNCTION do_test ()
// #include "../test-skeleton.c"
