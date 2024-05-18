#include "bootloader.h"
#include "port_vibl.h"
#include "nrfx_nvmc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void leave_hid_bl(void)
{
    dfu_update_status_t update_status;
    memset(&update_status, 0, sizeof(dfu_update_status_t));

    update_status.status_code = DFU_UPDATE_APP_COMPLETE;
    bootloader_dfu_update_process(update_status);
}

void hid_bl_write_flash(uint32_t start_add, uint8_t* data, uint16_t size)
{
    nrfx_nvmc_words_write(start_add, (uint32_t*)data, size / 4);
}

void hid_bl_read_flash(uint32_t start_add, uint8_t* buffer, uint16_t size)
{
}

void hid_bl_erase_flash(uint32_t start_add)
{
    nrfx_nvmc_page_erase(start_add);
}
