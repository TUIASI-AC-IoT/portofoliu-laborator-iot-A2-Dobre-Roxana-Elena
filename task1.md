# Laboratory 1

Refer to the documentation and answer the following questions

## What is the purpose of the gpio config function?

The gpio configuration function in microcontroller programming is used to set up GPIO pins for specific tasks like input, output, or alternative functions. It helps define how each pin behaves (e.g., digital read/write, analog input, etc.).

## In the example code, GPIO4 is configured as an output. What other modes can a GPIO pin be configured in?

Besides output, GPIO pins can be configured as input (reading values), pull-up/pull-down resistors (for stable input signals), analog input/output, or even for specific functions like PWM, SPI, I2C, UART, etc.

## Explain the vTaskDelay function call.
This function is part of FreeRTOS and is used to pause or delay the execution of a task for a specified period (in ticks). It helps manage task scheduling in real-time operating systems.

## Why is the main function called app main?
The main function is named 'app_main()' instead of 'main()' because the 'main' function is used internally already, having a default and pre-estabilished