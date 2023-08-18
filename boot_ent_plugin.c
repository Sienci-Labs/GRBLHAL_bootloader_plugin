/*

  boot_ent_plugin.c

  Part of grblHAL
  grblHAL is
  Copyright (c) 2022-2023 Terje Io

  Plugin code is
  Copyright (c) 2023 Expatria Technologies


  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <math.h>
#include <string.h>
#include <stdio.h>

#include "driver.h"

#if BOARD_LONGBOARD32
#include "../USB_DEVICE/App/usbd_cdc_if.h"
#include "../USB_DEVICE/App/usb_device.h"

#include "stm32f4xx_hal_flash.h"

#endif

#include "boot_ent_plugin.h"

static on_report_options_ptr on_report_options;

status_code_t enter_dfu (sys_state_t state, char *args)
{

hal.stream.write("[MSG:Warning: Entering DFU Bootloader]" ASCII_EOL);
hal.delay_ms(100, NULL);
#if BOARD_LONGBOARD32

    __disable_irq();
    *((unsigned long *)0x2003FFF0) = 0xDEADBEEF;
    __disable_irq();
    NVIC_SystemReset();

#endif

return Status_OK;
}

status_code_t enter_uf2 (sys_state_t state, char *args)
{


hal.stream.write("Entering UF2 Bootloader" ASCII_EOL);
return Status_OK;
}

const sys_command_t boot_command_list[] = {
    {"DFU", On, enter_dfu},
	{"UF2", On, enter_uf2}
};

static sys_commands_t boot_commands = {
    .n_commands = sizeof(boot_command_list) / sizeof(sys_command_t),
    .commands = boot_command_list
};

sys_commands_t *boot_get_commands()
{
    return &boot_commands;
}

static void report_options (bool newopt)
{
    on_report_options(newopt);

    if(!newopt)
        hal.stream.write("[PLUGIN:Bootloader Entry v0.01]" ASCII_EOL);
}

static void warning_msg (uint_fast16_t state)
{
    report_message("Selected board has no bootloader", Message_Warning);
}


void my_plugin_init (void)
{
    
	on_report_options = grbl.on_report_options;
	grbl.on_report_options = report_options;

    boot_commands.on_get_commands = grbl.on_get_commands;
    grbl.on_get_commands = boot_get_commands;

}
