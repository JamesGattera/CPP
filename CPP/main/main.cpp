// /*
// main.cpp
// */
//  ##  Include 'Packages'  ##
// #include <stdio.h>              // for printf ?
#include "freertos/FreeRTOS.h"  // for delays, tasks
//  ##  Outside 'Modules'   ##
#include "boot_sequence.h"      //Async Boot Process.h
// start of code-code ;;
extern "C" void app_main(void) //extern"C" only on app_main
{   //boot process runs asynchronously, 
    //without blocking
    xTaskCreate([](void *pv) {boot_sequence(); vTaskDelete(NULL); }, "bootseq", 2048, NULL, 5, NULL);
    printf("x   Main Banner \n");  //temp ; readability
    vTaskDelay(pdMS_TO_TICKS(1)); //tiny process break
    vTaskDelete(NULL);  // NULL means "delete this task"
}