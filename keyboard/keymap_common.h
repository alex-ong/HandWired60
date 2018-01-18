#ifndef KEYMAP_COMMON_H
#define KEYMAP_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"


extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const uint16_t fn_actions[];

#define KEYMAP( \
    K000, K001, K002, K003, K004, K005, K006, K007, K008, K009, K010, K011, K012, K013, \
    K100, K101, K102, K103, K104, K105, K106, K107, K108, K109, K110, K111, K112, K113, \
    K200, K201, K202, K203, K204, K205, K206, K207, K208, K209, K210, K211,       K213, \
    K300, K301, K302, K303, K304, K305, K306, K307, K308, K309, K310,             K313, \
    K400, K401, K402,             K405,                         K410, K411, K412, K413  \
) { \
    { KC_##K000, KC_##K001, KC_##K002, KC_##K003, KC_##K004, KC_##K005, KC_##K006, KC_##K007, KC_##K008, KC_##K009, KC_##K010, KC_##K011, KC_##K012, KC_##K013, }, \
    { KC_##K100, KC_##K101, KC_##K102, KC_##K103, KC_##K104, KC_##K105, KC_##K106, KC_##K107, KC_##K108, KC_##K109, KC_##K110, KC_##K111, KC_##K112, KC_##K113, }, \
    { KC_##K200, KC_##K201, KC_##K202, KC_##K203, KC_##K204, KC_##K205, KC_##K206, KC_##K207, KC_##K208, KC_##K209, KC_##K210, KC_##K211, KC_NO,     KC_##K213, }, \
    { KC_##K300, KC_##K301, KC_##K302, KC_##K303, KC_##K304, KC_##K305, KC_##K306, KC_##K307, KC_##K308, KC_##K309, KC_##K310, KC_NO,     KC_NO,     KC_##K313, }, \
    { KC_##K400, KC_##K401, KC_##K402, KC_NO,     KC_NO,     KC_##K405, KC_NO,     KC_NO,     KC_NO,     KC_NO,     KC_##K410, KC_##K411, KC_##K412, KC_##K413, }, \
}

#endif