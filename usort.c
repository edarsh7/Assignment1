/************************************************************************
 * 
 * CSE130 Winter 2021 Assignment 1
 * 
 * UNIX Shared Memory Multi-Process Merge Sort
 * 
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/

#include "merge.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 

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
 * Uses fork() to concurrently run singleProcessMergeSort() on the left (parent) sub array and the right (child)
 * sub array before copying the shared memory back into the main process and merging the two sub arrays.
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  /*these two lines calculate the middle element, and the amount of elements on the RIGHT SIDE*/
  int middle = (left+right)/2;
  int shm_size = right-middle;
  
  /*size of shared memory segment = size of integer * number of elements in arraay*/
  int shmid = shmget(IPC_PRIVATE, sizeof(int) * (shm_size), 0666|IPC_CREAT);
  int *r_array =  (int *)shmat (shmid, (void*)0,0);
  

  /*right side of local memory copied into shared memory*/
  memcpy(r_array, arr + middle + 1, sizeof(int)*(shm_size));

  /*switch case from lecture that allows us to manipulate the child process and parent process*/
  switch(fork()){
    case -1:
      exit(-1);
    case 0:
      
      /*attach shared memory*/
      r_array = (int *)shmat(shmid, (void*)0,0);

      /*calling ms on right side i.e. the child process*/
      singleProcessMergeSort(r_array, 0, (shm_size-1));
      
      /*detach shared memory from child process and exit*/
      shmdt(r_array);
      exit(0);
    default:
      /* send parent process array i.e. left side to mergesort*/
      singleProcessMergeSort(arr, 0, middle);

      /*waiting for child to finish process before continuing*/
      wait(NULL);
      
      /*copying the shared memory segment over to the right side of the local memory*/
      memcpy(arr+middle+1, r_array, sizeof(int)*(shm_size));

      /*detaching, delete and merge the local memory array*/
      shmdt(r_array);
      shmctl(shmid, IPC_RMID, NULL);
      merge(arr, left, middle, right);
  }
}
