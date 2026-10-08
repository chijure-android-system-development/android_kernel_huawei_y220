#include <linux/string.h>

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (320)
#define FRAME_HEIGHT (480)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

static __inline unsigned int HIGH_BYTE(unsigned int val)
{
    return (val >> 8) & 0xFF;
}

static __inline unsigned int LOW_BYTE(unsigned int val)
{
    return (val & 0xFF);
}

static __inline void send_ctrl_cmd(unsigned int cmd)
{
    lcm_util.send_cmd(cmd);
}

static __inline void send_data_cmd(unsigned int data)
{
    lcm_util.send_data(data);
}

static __inline unsigned int read_data_cmd()
{
    return lcm_util.read_data();
}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{
    send_ctrl_cmd(regIndex);
    send_data_cmd(regData);
}


static void init_lcm_registers(void)
{
    /*
     * ILI9488 320x480, DBI paralelo 16-bit (0x3A = 0x55).
     * Secuencia de Bodmer TFT_eSPI, TFT_Drivers/ILI9488_Init.h,
     * rama TFT_PARALLEL_16_BIT. La misma tabla está en
     * lvgl/lvgl_esp32_drivers y en el driver STM32 de RobertoBenjami.
     */
    send_ctrl_cmd(0xE0);
    send_data_cmd(0x00); send_data_cmd(0x03); send_data_cmd(0x09);
    send_data_cmd(0x08); send_data_cmd(0x16); send_data_cmd(0x0A);
    send_data_cmd(0x3F); send_data_cmd(0x78); send_data_cmd(0x4C);
    send_data_cmd(0x09); send_data_cmd(0x0A); send_data_cmd(0x08);
    send_data_cmd(0x16); send_data_cmd(0x1A); send_data_cmd(0x0F);

    send_ctrl_cmd(0xE1);
    send_data_cmd(0x00); send_data_cmd(0x16); send_data_cmd(0x19);
    send_data_cmd(0x03); send_data_cmd(0x0F); send_data_cmd(0x05);
    send_data_cmd(0x32); send_data_cmd(0x45); send_data_cmd(0x46);
    send_data_cmd(0x04); send_data_cmd(0x0E); send_data_cmd(0x0D);
    send_data_cmd(0x35); send_data_cmd(0x37); send_data_cmd(0x0F);

    send_ctrl_cmd(0xC0); send_data_cmd(0x17); send_data_cmd(0x15);
    send_ctrl_cmd(0xC1); send_data_cmd(0x41);
    send_ctrl_cmd(0xC5); send_data_cmd(0x00); send_data_cmd(0x12); send_data_cmd(0x80);
    send_ctrl_cmd(0x36); send_data_cmd(0x48);
    send_ctrl_cmd(0x3A); send_data_cmd(0x55);
    send_ctrl_cmd(0xB0); send_data_cmd(0x00);
    send_ctrl_cmd(0xB1); send_data_cmd(0xA0);
    send_ctrl_cmd(0xB4); send_data_cmd(0x02);
    send_ctrl_cmd(0xB6); send_data_cmd(0x02); send_data_cmd(0x02); send_data_cmd(0x3B);
    send_ctrl_cmd(0xB7); send_data_cmd(0xC6);
    send_ctrl_cmd(0xF7); send_data_cmd(0xA9); send_data_cmd(0x51);
    send_data_cmd(0x2C); send_data_cmd(0x82);

    send_ctrl_cmd(0x35);
    send_data_cmd(0x00);
    send_ctrl_cmd(0x11);
    MDELAY(120);
    send_ctrl_cmd(0x29);
    MDELAY(25);
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DBI;
    params->ctrl   = LCM_CTRL_PARALLEL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 3;

    params->dbi.port                    = 1;
    params->dbi.clock_freq              = LCM_DBI_CLOCK_FREQ_26M;
    params->dbi.te_mode                 = LCM_DBI_TE_MODE_DISABLED;
    params->dbi.data_width              = LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dbi.data_format.trans_seq   = LCM_DBI_TRANS_SEQ_MSB_FIRST;
    params->dbi.data_format.padding     = LCM_DBI_PADDING_ON_LSB;
    params->dbi.data_format.format      = LCM_DBI_FORMAT_RGB565;
    params->dbi.data_format.width       = LCM_DBI_DATA_WIDTH_16BITS;
    params->dbi.cpu_write_bits          = LCM_DBI_CPU_WRITE_16_BITS;
    params->dbi.io_driving_current      = 0;

    params->dbi.parallel.write_setup    = 1;
    params->dbi.parallel.write_hold     = 1;
    params->dbi.parallel.write_wait     = 6;
    params->dbi.parallel.read_setup     = 1;
    params->dbi.parallel.read_latency   = 31;
    params->dbi.parallel.wait_period    = 2;
}


static void lcm_init(void)
{
    SET_RESET_PIN(0);
    MDELAY(200);
    SET_RESET_PIN(1);
    MDELAY(400);

    init_lcm_registers();
}


static void lcm_suspend(void)
{
	send_ctrl_cmd(0x28);
}


static void lcm_resume(void)
{
	send_ctrl_cmd(0x29);
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

	send_ctrl_cmd(0x2A);
	send_data_cmd(HIGH_BYTE(x0));
	send_data_cmd(LOW_BYTE(x0));
	send_data_cmd(HIGH_BYTE(x1));
	send_data_cmd(LOW_BYTE(x1));

	send_ctrl_cmd(0x2B);
	send_data_cmd(HIGH_BYTE(y0));
	send_data_cmd(LOW_BYTE(y0));
	send_data_cmd(HIGH_BYTE(y1));
	send_data_cmd(LOW_BYTE(y1));

	// Write To GRAM
	send_ctrl_cmd(0x2C);
}


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9488_dbi_lcm_drv =
{
    .name			= "ili9488_dbi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
};
