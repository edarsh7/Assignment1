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
  int middle = (left+right)/2;
  int size_value = right-middle;
  
  //size of shared memory segment = size of integer * number of elements in arraay
  int shmid = shmget(IPC_PRIVATE, sizeof(int) * size_value, 0666|IPC_CREAT);
  int *r_array =  (int *)shmat (shmid, (void*)0,0);
  

  // right side of local memory copied into shared memory
  memcpy(r_array, arr + middle + 1, sizeof(int)*(right-middle));

  switch(fork()){
    case -1:
      exit(-1);
    case 0:
      r_array = (int *)shmat(shmid, (void*)0,0);
      singleProcessMergeSort(r_array, 0, (right-middle-1));
      shmdt(r_array);
      exit(0);
    default:
      singleProcessMergeSort(arr, 0, middle);
      wait(NULL);
      memcpy(arr+middle+1, r_array, sizeof(int)*(right-middle));
      shmdt(r_array);
      shmctl(shmid, IPC_RMID, NULL);
      merge(arr, left, middle, right);
  }
}
