#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "semphr.h"

#define LED_PIN 12
#define BUTTON_B 6

QueueHandle_t dataQueue;

bool isButtonPressed(uint8_t buttonPin) {
    return gpio_get(buttonPin) == 0;
}

void vTask1(void *pvParameters) {
    bool buttonPressed = false;

    for (;;) {
        buttonPressed = isButtonPressed(BUTTON_B);
        xQueueSend(dataQueue, &buttonPressed, portMAX_DELAY);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void vTask2(void *pvParameters) {
    bool buttonPressed;

    for (;;) {
        if (xQueueReceive(dataQueue, &buttonPressed, portMAX_DELAY) == pdTRUE) {
            if (buttonPressed) {
                xQueueSend(dataQueue, &buttonPressed, portMAX_DELAY);
            }
        }
    }
}

void vTask3(void *pvParameters) {
    bool buttonPressed;

    for (;;) {
        if (xQueueReceive(dataQueue, &buttonPressed, portMAX_DELAY) == pdTRUE) {
            gpio_put(LED_PIN, buttonPressed);
        }
    }
}

void setup(void) {
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
};

int main(void) {
    setup();

    dataQueue = xQueueCreate(5, sizeof(float));

    if (dataQueue != NULL) {
        xTaskCreate(vTask1, "Task 1", 128, NULL, 1, NULL);
        xTaskCreate(vTask2, "Task 2", 128, NULL, 1, NULL);
        xTaskCreate(vTask3, "Task 3", 128, NULL, 1, NULL);
        vTaskStartScheduler();
    }
}
