#ifndef CONFIG_H
#define CONFIG_H


/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6060
#define DEVICE_VER      0x0002
#define MANUFACTURER    XeaL
#define PRODUCT         XeaLouS Keyboard
#define DESCRIPTION     XeaLouS Keyboard

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 13

#define NO_ACTION_ONESHOT
#define NO_ACTION_TAPPING
#define NO_ACTION_MACRO
#define NO_TRACK_KEY_PRESS //Try this out for 2016 era layer+shift detection (faster). 

/* key combination for command */
#define IS_COMMAND() (keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)))

#endif