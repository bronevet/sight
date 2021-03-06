/******************************************************************************
* FILE: bug1.c
* DESCRIPTION:
*   This example has a bug. It is a variation on the condvar.c example. 
*   Instead of just one thread waiting for the condition signal, there are
*   four threads waiting for the same signal. Find out how to fix the
*   program. The solution program is bug1fix.c.
* SOURCE: Adapted from example code in "Pthreads Programming", B. Nichols
*   et al. O'Reilly and Associates.
* LAST REVISED: 07/06/05  Blaise Barney
******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
// Written by the Greg Bronevetsky <bronevetsky1@llnl.gov> / <greg@bronevetsky.com>. 
//
// LLNL-CODE-642002
// All rights reserved.
//
// This file is part of Sight. For details, see https://e-reports-ext.llnl.gov/pdf/781752.pdf or
// https://github.com/bronevet/sight.
//
// Licensed under the GNU Lesser General Public License (Lesser GPU) Version 2.1, 
// February 1999; you may not use this file except in compliance with the License.  
// The full licence is included in file LICENCE and you may obtain a copy of the 
// License at: 
// https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the license.
////////////////////////////////////////////////////////////////////////////////

#include "sight_pthread.h"
using namespace sight;
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS  6
#define TCOUNT 10
#define COUNT_LIMIT 12

int     sharedCount = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void *inc_count(void *idp) 
{
  scope s("inc_count");
  int j,i;
  double result=0.0;
  long my_id = (long)idp;
  for (i=0; i < TCOUNT; i++) {
    pthread_mutex_lock(&count_mutex);
    sharedCount++;
    scope(txt()<<"sharedCount="<<sharedCount);

    /* 
    Check the value of sharedCount and signal waiting thread when condition is
    reached.  Note that this occurs while mutex is locked. 
    */
    if (sharedCount == COUNT_LIMIT) {
      pthread_cond_signal(&count_threshold_cv);
      dbgprintf("inc_count(): thread %ld, sharedCount = %d  Threshold reached.\n", my_id, sharedCount);
      }
    dbgprintf("inc_count(): thread %ld, sharedCount = %d, unlocking mutex\n", my_id, sharedCount);
    pthread_mutex_unlock(&count_mutex);

    /* Do some work so threads can alternate on mutex lock */
    sleep(1);
    }
  pthread_exit(NULL);
}

void *watch_count(void *idp) 
{
  long my_id = (long)idp;
  scope(txt()<<"watch_count() my_id="<<my_id);

  /*
  Lock mutex and wait for signal.  Note that the pthread_cond_wait routine
  will automatically and atomically unlock mutex while it waits. 
  Also, note that if COUNT_LIMIT is reached before this routine is run by
  the waiting thread, the loop will be skipped to prevent pthread_cond_wait
  from never returning.
  */
  pthread_mutex_lock(&count_mutex);
    dbgprintf("***Before cond_wait: thread %ld\n", my_id);
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    dbgprintf("***Thread %ld Condition signal received. sharedCount=%d\n", my_id, sharedCount);
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int i, rc;
  pthread_t threads[6];
  pthread_attr_t attr;

  SightInit(argc, argv, "Pthreads Bug1", "dbg.pthreadBug1.individual");

  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init (&count_threshold_cv, NULL);

  /*
  For portability, explicitly create threads in a joinable state 
  */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  dbg << "Starting threads"<<endl;
  pthread_create(&threads[2], &attr, watch_count, (void *)2);
  pthread_create(&threads[3], &attr, watch_count, (void *)3);
  pthread_create(&threads[4], &attr, watch_count, (void *)4);
  pthread_create(&threads[5], &attr, watch_count, (void *)5);
  pthread_create(&threads[0], &attr, inc_count, (void *)0);
  pthread_create(&threads[1], &attr, inc_count, (void *)1);

  /* Wait for all threads to complete */
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  dbgprintf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit (NULL);

}

