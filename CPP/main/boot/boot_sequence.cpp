/*
boot_sequence.cpp
Handles initial boot-sequence/ banner.
*/
#include <stdio.h>              // printf
#include "freertos/FreeRTOS.h"  //delays, tasks
#include "freertos/task.h"      // Task management
/*
Print a boot message, wait a tick, delete self.
*/
#define TINY_DELAY_MS 1
void boot_sequence()
{
    printf("x   Boot Banner \n");  //temp ; readability
    vTaskDelay(pdMS_TO_TICKS(TINY_DELAY_MS)); //tiny process break
    //expanded tasks here
    vTaskDelete(NULL);  // NULL means "delete this task"
}