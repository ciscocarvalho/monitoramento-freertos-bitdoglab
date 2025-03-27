#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

const uint led_pin = 12;

void vBlinkTask() {
    for (;;) {
        gpio_put(led_pin, 1);

        vTaskDelay(250);

        gpio_put(led_pin, 0);

        vTaskDelay(250);

        printf("Blinking\n");
    }
}

int main() {
    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    xTaskCreate(vBlinkTask, "Blink Task", 128, NULL, 1, NULL);
    vTaskStartScheduler();
}
