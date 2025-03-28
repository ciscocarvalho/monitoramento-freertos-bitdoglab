#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"

QueueHandle_t dataQueue;

void vSensorTask(void *pvParameters) {
    float sensorData = 0;

    for (;;) {
        sensorData = read_onboard_temperature();
        xQueueSend(dataQueue, &sensorData, portMAX_DELAY);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vProcessingTask(void *pvParameters) {
    float receivedData;
    static uint cnt = 0;
    static float avg = 0;

    for (;;) {
        if (xQueueReceive(dataQueue, &receivedData, portMAX_DELAY) == pdTRUE) {
            cnt++;
            avg += receivedData;

            if (cnt == 10) {
                cnt = 0;
                avg /= 10;
                printf("Media: %f\n", avg);
                avg = 0;
            }
        }
    }
}

void setup(void) {
    stdio_init_all();
    adc_init();
    adc_set_temp_sensor_enabled(true);
};

int main(void) {
    stdio_init_all();

    setup();

    dataQueue = xQueueCreate(5, sizeof(float));

    if (dataQueue != NULL) {
        xTaskCreate(vSensorTask, "Sensor Task", 128, NULL, 1, NULL);
        xTaskCreate(vProcessingTask, "Processing Task", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
        vTaskStartScheduler();
    }
}
