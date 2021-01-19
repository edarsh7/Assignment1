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
  int *array =  (int *)shmat (shmid, (void*)0,0);

  // we have attached arr to array ptr with memcpy
  memcpy(array, arr + middle + 1, 5*sizeof(arr));

  switch(fork()){
    case -1:
      exit;
    // this is the child process that has array2, and array
    case 0:
      array = (int *)shmat (shmid, (void*)0,0);
      printf("child copied array[0]: %d\n", array[0]);
      break;
    default:
      wait(NULL);
      shmdt(array);
      shmctl(shmid, IPC_RMID,NULL);
  }
}
