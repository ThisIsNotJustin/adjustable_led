#include <pigpio.h>
#include <stdio.h>

#define POT_PIN 26
#define LED_PIN 16

int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize pigpio\n");
        return 1;
    }

    gpioSetMode(POT_PIN, PI_INPUT);
    gpioSetMode(LED_PIN, PI_OUTPUT);

    while (1) {
        int values = gpioRead(POT_PIN);
        gpioPWM(LED_PIN, values*255);
        usleep(10000);
    }

    gpioTerminate();
    return 0;
}