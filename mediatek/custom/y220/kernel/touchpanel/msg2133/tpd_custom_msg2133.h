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

#endif
