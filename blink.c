#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"

SemaphoreHandle_t xMutex;
int sharedCounter = 0;

void vTask1(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            sharedCounter++;
            printf("Task 1: Counter = %d\n", sharedCounter);
            xSemaphoreGive(xMutex);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            sharedCounter++;
            printf("Task 2: Counter = %d\n", sharedCounter);
            xSemaphoreGive(xMutex);
        }

        vTaskDelay(700 / portTICK_PERIOD_MS);
    }
}

int main(void) {
    stdio_init_all();

    xMutex = xSemaphoreCreateMutex();

    if (xMutex != NULL) {
        xTaskCreate(vTask1, "Task 1", 256, NULL, 1, NULL);
        xTaskCreate(vTask2, "Task 2", 256, NULL, 1, NULL);
        vTaskStartScheduler();
    }
}
