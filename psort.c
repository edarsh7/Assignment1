/************************************************************************
 * 
 * CSE130 Winter 2021 Assignment 1
 *  
 * POSIX Shared Memory Multi-Process Merge Sort
 * 
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/

#include "merge.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/shm.h> 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/ipc.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>


/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * This function stub needs to be completed
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  /*these two lines calculate the middle element, and the amount of elements on the RIGHT SIDE*/
  int middle = (left+right)/2;
  int shm_size = right-middle;

  const char* name = "shm_obj";
  
  int shmid = shm_open(name, O_CREAT|O_RDWR, 0666);
  ftruncate(shmid, sizeof(int) * (shm_size));
  
  int* r_array = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shmid, 0);
  memcpy(r_array, arr + middle + 1, sizeof(int)*(shm_size));

  /*switch case from lecture that allows us to manipulate the child process and parent process*/
  switch(fork()){
    case -1:
      exit(-1);
    case 0:
      
      /*attach shared memory*/
      r_array = mmap(0, shm_size, PROT_WRITE, MAP_SHARED, shmid, 0);

      /*calling ms on right side i.e. the child process*/
      singleProcessMergeSort(r_array, 0, (shm_size-1));
      
      /*detach shared memory from child process and exit*/
      shm_unlink(name);
      exit(0);
    default:
      /* send parent process array i.e. left side to mergesort*/
      singleProcessMergeSort(arr, 0, middle);

      /*waiting for child to finish process before continuing*/
      wait(NULL);
      
      /*copying the shared memory segment over to the right side of the local memory*/
      memcpy(arr+middle+1, r_array, sizeof(int)*(shm_size));

      /*detaching, delete and merge the local memory array*/
      shm_unlink(name);
      close(shmid);
      merge(arr, left, middle, right);
  }
}


/*credits:
* https://www.geeksforgeeks.org/posix-shared-memory-api/  used to learn posix shared memory api
* Professor Harrisons lecture notes 4 pg 12 on unix shared memory and fork()
* https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/ShMem.html more material on posix shared memory
*/