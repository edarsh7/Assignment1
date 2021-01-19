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
  //we have an array
  int array[4] = {1,2,3,4};
  
  //we create a shared memory segment that has an ID called shmid
  int shmid = shmget(IPC_PRIVATE, 1024, 0666|IPC_CREAT);
  //we attach the address of the shared memory segment to an int pointer called array2
  int* array2 =  (int *)shmat (shmid, (void*)0,0);
  //we copy the elements in the array into the shared memory space called array2
  memcpy(array2, array, sizeof(array));
  printf("parent process: %d has array[0] as %d\n", getpid(), array[0]);

  //now we fork the process
  switch(fork()){
    case -1:
      exit;
    // this is the child process that has array2, and array
    case 0:
      array2[0] = 9;
      array[0] = 5;
      printf("child process: %d has array[0] as: %d\n\n", getpid(), array2[0]);
      printf("C P array[0] = %d", array[0]);
      break;
    default:
      array[0] = 4;
      printf("P P array[0] = %d", array[0]);
      wait(NULL);
      shmdt(array2);
      shmctl(shmid, IPC_RMID,NULL);
  }
}
