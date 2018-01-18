#include <avr/io.h>
#include "stdint.h"
#include "led.h"

void led_set(uint8_t usb_led) {
    if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
        DDRB  |=  (1 << 2);
        PORTB &= ~(1 << 2);
    } else {
        DDRB  &= ~(1 << 2);
        PORTB &= ~(1 << 2);
    }
}