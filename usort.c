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
 * This function stub needs to be completed
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  int middle = (left+right)/2;

  //int arr[]= {} is already created
  int shmid = shmget(IPC_PRIVATE, 4*sizeof(arr), 0666|IPC_CREAT);
  int *r_array =  (int *)shmat (shmid, (void*)0,0);
  

  // right side of local memory copied into shared memory
  memcpy(r_array, arr + middle + 1, sizeof(arr));

  switch(fork()){
    case -1:
      exit;
    case 0:
      r_array = (int *)shmat (shmid, (void*)0,0);
      int a_size = sizeof(r_array)/sizeof(r_array[0]);
      printf("xr_arr[0]=%d\n",r_array[1]);
      singleProcessMergeSort(r_array, 0, a_size - 1);
      shmdt(r_array);
      exit(0);
    default:
      singleProcessMergeSort(arr, 0, middle);
      wait(NULL);
      memcpy(arr+middle,r_array, sizeof(r_array));
      shmdt(r_array);
      shmctl(shmid, IPC_RMID,NULL);
      merge(arr, left, middle, right);
  }
}
