#include <pigpio.h>
#include <stdio.h>
#include <signal.h>

#define LED_PIN 16
#define ADC_ADDR 0x4b

volatile int running = 1;

void handle_sigint(int sig) {
    running = 0;
}

int readADC(int handle, int channel) {
    char controlByte = (channel << 4) | 0x80;
    char data[1];

    if (i2cWriteDevice(handle, &controlByte, 1) != 0) {
        fprintf(stderr, "Failed to write control byte\n");
        return -1;
    }

    if (i2cReadDevice(handle, data, 1) != 1) {
        fprintf(stderr, "Failed to read data\n");
        return -1;
    }

    return (int)data[0];
}

int main() {
    signal(SIGINT, handle_sigint);

    gpioCfgSetInternals(gpioCfgGetInternals() | PI_CFG_NOSIGHANDLER);

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Failed to initialize piGPIO\n");
        return 1;
    }

    int i2cHandle = i2cOpen(1, ADC_ADDR, 0);
    if (i2cHandle < 0) {
        fprintf(stderr, "Failed to open I2C\n");
        return 1;
    }

    gpioSetMode(LED_PIN, PI_OUTPUT);

    while (running) {
        int values = readADC(i2cHandle, 0);
        if (values < 0) {
            fprintf(stderr, "Error reading ADC\n");
            break;
        }
        
        gpioPWM(LED_PIN, values);
        usleep(10000);
    }

    printf("Terminating program\n");
    gpioPWM(LED_PIN, 0);
    i2cClose(i2cHandle);
    gpioTerminate();

    return 0;
}