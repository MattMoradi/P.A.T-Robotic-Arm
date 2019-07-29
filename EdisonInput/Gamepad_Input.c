/*
 * Copyright (c) Matthew Moradi 2019
 * Target:		Intel Edison i686
 * Gamepad:		Logitech Dual Action
 * Compilation: gcc -o gamepad gamepad.c -lmraa
 *
 * References: 	https://www.kernel.org/doc/Documentation/input/event-codes.txt
 * 				https://www.kernel.org/doc/Documentation/input/input.txt
 */

#include <stdio.h>
#include "mraa.h"
#include <sys/types.h>
#include <unistd.h>

#include <linux/input.h>

#include <string.h>

#define LED_PIN 13

int main() {

	mraa_gpio_context ledPin;

	// Detects running platform and attempts to use included pinmap
	mraa_init();

	// Initializes the GPIO context to ledPin 13
	ledPin = mraa_gpio_init(LED_PIN);

	// Sets said pin as output
	mraa_gpio_dir(ledPin, MRAA_GPIO_OUT);

	// Gamepad is mounted to event2 on Edison rather than js0
	int dag = open("/dev/input/event2", O_RDONLY);
	if (dag == 0){
		fprintf(stderr, "Could not open Dual Action Gamepad on event2\n");
		return EXIT_FAILURE;
	}
	fprintf(stdout, "Opened Dual Action Gamepad on event2\n");

	int nbytes;

	struct input_event event;

	// Gets event size which should be 16
	fprintf(stdout, "Size of event is: %d\n", sizeof(event));

	int counter = 0;

	char text[30];

	char valid;
	while (1){
		nbytes = read(dag, &event, sizeof(event));
		// Should read 16 bytes
		// If not, discard the data and try again
		if (nbytes < sizeof(event))
			continue;

		valid = 0;

		switch (event.type){
		case EV_KEY:
			switch (event.code){
			case BTN_BASE3:
				strcpy(text, "LED Turned ON");
				mraa_gpio_write(ledPin, 1);
				valid = 1;
				break;
			case BTN_BASE4:
				strcpy(text, "LED Turned OFF");
				mraa_gpio_write(ledPin, 0);
				valid = 1;
				break;
			}

			if (valid)
				fprintf(stdout, "[%d] %s\n", counter++, text);
			break;
		}
	}

	// Close the Dual Action Gamepad on event2
	close(dag);

	return EXIT_SUCCESS;
}
