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

#include "boot_ent_plugin.h"


status_code_t enter_dfu (sys_state_t state, char *args)
{


hal.stream.write("Entering DFU Bootloader" ASCII_EOL);
return Status_OK;
}

status_code_t enter_uf2 (sys_state_t state, char *args)
{


hal.stream.write("Entering UF2 Bootloader" ASCII_EOL);
return Status_OK;
}

const sys_command_t boot_command_list[] = {
    {"dfu", enter_dfu, { .noargs = On }}
	{"uf2", enter_uf2, { .noargs = On }}
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

    stream.write = NULL;

    //protocol_enqueue_rt_command(warning_msg);
}

#else

void my_plugin_init (void)
{
    if(hal.port.num_digital_out > 0) {

        relay_port = --hal.port.num_digital_out;        // "Claim" the port, M62-M65 cannot be used
//        relay_port = hal.port.num_digital_out - 1;    // Do not "claim" the port, M62-M65 can be used

        if(hal.port.set_pin_description)
            hal.port.set_pin_description(Port_Digital, Port_Output, relay_port, "Probe relay");

        memcpy(&user_mcode, &hal.user_mcode, sizeof(user_mcode_ptrs_t));

        hal.user_mcode.check = mcode_check;
        hal.user_mcode.validate = mcode_validate;
        hal.user_mcode.execute = mcode_execute;

        driver_reset = hal.driver_reset;
        hal.driver_reset = probe_reset;

        on_report_options = grbl.on_report_options;
        grbl.on_report_options = report_options;

        grbl.on_probe_fixture = probe_fixture;

    } else
        protocol_enqueue_rt_command(warning_msg);
}

#endif