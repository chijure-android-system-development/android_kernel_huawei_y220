#ifndef TPD_CUSTOM_MSG2133_H
#define TPD_CUSTOM_MSG2133_H

#include <mach/mt_pm_ldo.h>

#define TPD_TYPE_CAPACITIVE
#define TPD_RES_X                320
#define TPD_RES_Y                480
#define MAX_TOUCH_FINGER         2
#define REPORT_PACKET_LENGTH     8
#define MS_TS_MSG21XX_X_MAX      320
#define MS_TS_MSG21XX_Y_MAX      480
#define REVERSE_Y

#define TPD_POWER_SOURCE         MT6323_POWER_LDO_VMC
#define TPD_POWER_SOURCE_CUSTOM  MT6323_POWER_LDO_VMC
#define TPD_I2C_NUMBER           1

/*
 * Capacitive keys sit outside the 320x480 panel. The controller
 * reports them as key codes 1, 2 and 4, which the driver places at
 * these points. tpd_button() matches them; they are not screen touches.
 * Order is left to right: menu, home, back.
 */
#define TPD_HAVE_BUTTON
#define TPD_KEY_COUNT            3
#define TPD_KEYS                 { KEY_MENU, KEY_HOME, KEY_BACK }
#define TPD_KEYS_DIM             {{80,850,120,80},{240,850,120,80},{400,850,120,80}}

#endif
