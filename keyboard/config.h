#ifndef CONFIG_H
#define CONFIG_H


/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6060
#define DEVICE_VER      0x0002
#define MANUFACTURER    XeaL
#define PRODUCT         XeaLouS Keyboard
#define DESCRIPTION     Custom Keyboard

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 13


/* Key-presses are sent immediately. Debounce is how long before we are allowed to check for next signal */
#define DEBOUNCE_RELEASE 5 //After releasing, how long before we can check for key-up?
#define DEBOUNCE_PRESS 5 //After pressing, how long before we can check for key-down?

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() (keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)))

#endif