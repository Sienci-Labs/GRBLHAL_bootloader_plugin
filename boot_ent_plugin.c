/*

  boot_ent_plugin.c

  Part of grblHAL
  grblHAL is
  Copyright (c) 2022-2023 Terje Io

  DFU entry code is

   Copyright (C) Sienci Labs Inc.
  
   This file is part of the SuperLongBoard family of products.
  
   This source describes Open Hardware and is licensed under the "CERN-OHL-S v2"

   You may redistribute and modify this source and make products using
   it under the terms of the CERN-OHL-S v2 (https://ohwr.org/cern_ohl_s_v2.t). 
   This source is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY,
   INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A 
   PARTICULAR PURPOSE. Please see the CERN-OHL-S v2 for applicable conditions.
   
   As per CERN-OHL-S v2 section 4, should You produce hardware based on this 
   source, You must maintain the Source Location clearly visible on the external
   case of the CNC Controller or other product you make using this source.
  
   You should have received a copy of the CERN-OHL-S v2 license with this source.
   If not, see <https://ohwr.org/project/cernohl/wikis/Documents/CERN-OHL-version-2>.
   
   Contact for information regarding this program and its license
   can be sent through gSender@sienci.com or mailed to the main office
   of Sienci Labs Inc. in Waterloo, Ontario, Canada.

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

const sys_command_t boot_command_list[2] = {
    {"DFU", enter_dfu, { .noargs = On }},
	{"UF2", enter_uf2, { .noargs = On }}
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


void boot_entry_init (void)
{
    
	on_report_options = grbl.on_report_options;
	grbl.on_report_options = report_options;

    boot_commands.on_get_commands = grbl.on_get_commands;
    grbl.on_get_commands = boot_get_commands;

}
