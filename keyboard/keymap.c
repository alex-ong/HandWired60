#include "keymap_common.h"

const uint8_t NUM_LAYERS = 2;

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* layer 0 */
    KEYMAP(
         ESC,    1,    2,    3,    4,    5,    6,    7,    8,    9,    0, MINS,  EQL, \
         TAB,    Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P, LBRC, RBRC, \
         FN0,    A,    S,    D,    F,    G,    H,    J,    K,    L, SCLN, QUOT,  ENT, \
        LSFT,    Z,    X,    C,    V,    B,    N,    M, COMM,  DOT, SLSH, RSFT, BSLS, \
        LCTL, LGUI, LALT,              SPC, RALT,  FN0,  APP, RCTL,             BSPC  ),
    /* layer 1 */
    KEYMAP(
         GRV,   F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,  F10,  F11,  F12, \
        CAPS, TRNS,   UP, TRNS, TRNS, TRNS, TRNS, PGUP,   UP, PGDN, PSCR, SLCK, PAUS, \
        TRNS, LEFT, DOWN, RGHT, TRNS, TRNS, HOME, LEFT, DOWN, RGHT,  INS,  DEL, TRNS, \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,  END, TRNS, VOLD, VOLU, MUTE, TRNS, TRNS, \
        TRNS, TRNS, TRNS,             TRNS, TRNS, TRNS, TRNS, TRNS,              DEL  ),    
};

const action_t PROGMEM fn_actions[] = {
    [0]  = ACTION_LAYER_MOMENTARY(1)
};