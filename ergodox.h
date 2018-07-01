/*
Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Copyright (c) 2012, 2013 Ben Blazak <benblazak.dev@gmail.com>
Released under The MIT License (see "doc/licenses/MIT.md")
Project located at <https://github.com/benblazak/ergodox-firmware>

Most used files are located at
<https://github.com/benblazak/ergodox-firmware/tree/partial-rewrite/firmware/keyboard/ergodox/controller>

*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "i2cmaster.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00

// I2C aliases and register addresses (see "mcp23018.md")
#define I2C_ADDR        0b0100000
#define I2C_ADDR_WRITE  ( (I2C_ADDR<<1) | I2C_WRITE )
#define I2C_ADDR_READ   ( (I2C_ADDR<<1) | I2C_READ  )
#define IODIRA          0x00            // i/o direction register
#define IODIRB          0x01
#define GPPUA           0x0C            // GPIO pull-up resistor register
#define GPPUB           0x0D
#define GPIOA           0x12            // general purpose i/o port register (write modifies OLAT)
#define GPIOB           0x13
#define OLATA           0x14            // output latch register
#define OLATB           0x15

extern uint8_t mcp23018_status;

void init_ergodox(void);
void ergodox_blink_all_leds(void);
uint8_t init_mcp23018(void);

#define LED_BRIGHTNESS_LO       31
#define LED_BRIGHTNESS_HI       100

inline void __attribute__((always_inline)) ergodox_board_led_on(void)      { DDRD |=  (1<<6); PORTD |=  (1<<6); }
inline void __attribute__((always_inline)) ergodox_right_led_1_on(void)    { DDRB |=  (1<<5); PORTB |=  (1<<5); }
inline void __attribute__((always_inline)) ergodox_right_led_2_on(void)    { DDRB |=  (1<<6); PORTB |=  (1<<6); }
inline void __attribute__((always_inline)) ergodox_right_led_3_on(void)    { DDRB |=  (1<<7); PORTB |=  (1<<7); }

inline void __attribute__((always_inline)) ergodox_board_led_off(void)     { DDRD &= ~(1<<6); PORTD &= ~(1<<6); }
inline void __attribute__((always_inline)) ergodox_right_led_1_off(void)   { DDRB &= ~(1<<5); PORTB &= ~(1<<5); }
inline void __attribute__((always_inline)) ergodox_right_led_2_off(void)   { DDRB &= ~(1<<6); PORTB &= ~(1<<6); }
inline void __attribute__((always_inline)) ergodox_right_led_3_off(void)   { DDRB &= ~(1<<7); PORTB &= ~(1<<7); }

inline void __attribute__((always_inline))ergodox_led_all_on(void)
{
    ergodox_board_led_on();
    ergodox_right_led_1_on();
    ergodox_right_led_2_on();
    ergodox_right_led_3_on();
}

inline void __attribute__((always_inline))ergodox_led_all_off(void)
{
    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
}

inline void __attribute__((always_inline))ergodox_right_led_1_set(uint8_t n)    { OCR1A = n; }
inline void __attribute__((always_inline))ergodox_right_led_2_set(uint8_t n)    { OCR1B = n; }
inline void __attribute__((always_inline))ergodox_right_led_3_set(uint8_t n)    { OCR1C = n; }

inline void __attribute__((always_inline))ergodox_led_all_set(uint8_t n)
{
    ergodox_right_led_1_set(n);
    ergodox_right_led_2_set(n);
    ergodox_right_led_3_set(n);
}

