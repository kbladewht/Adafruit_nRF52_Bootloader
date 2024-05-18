#include <stdint.h>
#include <string.h>
#include "port_vibl.h"
#include "flash_nrf5x.h"
#include "boards.h"
#include "class/hid/hid_device.h"

#define FLASH_CACHE_INVALID_ADDR 0xffffffff

#ifndef FLASH_BUFFER_WAY
#define FLASH_BUFFER_WAY
#endif

#if 0
static uint32_t fl_addr = FLASH_CACHE_INVALID_ADDR;
static uint8_t fl_buf[FLASH_MIN_ERASE_SIZE] __attribute__((aligned(4)));
#endif

enum {
    STATE_INIT = 0,
    STATE_FLASH,
};

__attribute__((weak)) void leave_hid_bl(void)
{
}

__attribute__((weak)) void hid_bl_write_flash(uint32_t start_add, uint8_t* data, uint16_t size)
{
}

__attribute__((weak)) void hid_bl_read_flash(uint32_t start_add, uint8_t* buffer, uint16_t size)
{
}

__attribute__((weak)) void hid_bl_erase_flash(uint32_t start_add)
{
}

static void hid_bl_flush_flash(void)
{
#if 0
    if (fl_addr == FLASH_CACHE_INVALID_ADDR)
        return;

    // skip the write if contents matches
    if (memcmp(fl_buf, (void*)fl_addr, FLASH_MIN_ERASE_SIZE) != 0) {
        PRINTF("Erase and ");
        hid_bl_erase_flash(fl_addr);

        PRINTF("Write 0x%08lX\r\n", fl_addr);
        hid_bl_write_flash(fl_addr, fl_buf, FLASH_MIN_ERASE_SIZE);
    }

    fl_addr = FLASH_CACHE_INVALID_ADDR;
#endif
    flash_nrf5x_flush(true);
}

static void hid_bl_write_flash_buffer(uint32_t start_add, uint8_t* data, uint16_t size)
{
#if 0
    /*!< newAddr = dst & 0xfffff000 */
    uint32_t newAddr = start_add & ~(FLASH_MIN_ERASE_SIZE - 1);
    if (newAddr != fl_addr) {
        hid_bl_flush_flash();
        fl_addr = newAddr;
        memcpy(fl_buf, (void*)newAddr, FLASH_MIN_ERASE_SIZE);
    }
    /*!< dst & 0x00fff */
    memcpy(fl_buf + (start_add & (FLASH_MIN_ERASE_SIZE - 1)), data, size);
#endif
    flash_nrf5x_write(start_add, data, size, true);
}

static uint8_t vibl_packet_is_command(const uint8_t* page)
{
    return (page[0] == 'V' && page[1] == 'C');
}

void hid_vibl_handler(uint8_t* data)
{
    static int state = STATE_INIT;
    static uint32_t pagesToFlash;
    static uint32_t currentPage;
    static uint32_t currentPageOffset;

    /* Will flash 64 bytes at a time */
    static uint8_t pageData[64];

    static uint8_t keyboard_id[8] = VIAL_KEYBOARD_UID;

    static uint8_t bootloader_ident[8] = { 1 };

    if (state == STATE_INIT) {
        for (size_t i = 0; i < 64; ++i)
            pageData[currentPageOffset + i] = data[i];
        currentPageOffset += 64;

        if (currentPageOffset == sizeof(pageData)) {
            currentPageOffset = 0;

            if (vibl_packet_is_command(pageData)) {
                switch (pageData[2]) {
                case 0x00:
                    /* Retrieve bootloader version, flags */
                    PRINTF("Retrieve bootloader version, flags\r\n");
                    tud_hid_report(0, bootloader_ident, sizeof(bootloader_ident));
                    break;
                case 0x01:
                    /* Send vial keyboard ID */
                    PRINTF("Send vial keyboard ID\r\n");
                    tud_hid_report(0, keyboard_id, sizeof(keyboard_id));
                    break;
                case 0x02:
                    /* Flash */
                    currentPage = 0;
                    pagesToFlash = pageData[3] + 256 * pageData[4];
                    PRINTF("pagesToFlash %ld\r\n", pagesToFlash);
                    /* Don't allow to pass a ridiculous value. 10 megs max */
                    if (pagesToFlash < 10 * 1024 * 1024 / sizeof(pageData)) {
                        state = STATE_FLASH;
                        currentPageOffset = 0;
                    }
                    break;
                case 0x03:
                    /* Reboot */
                    leave_hid_bl();
                    break;
                case 0x04:
                    /* set insecure so that on first boot we can restore layout */
                    // setInsecureFlag();
                    break;
                default:
                    break;
                }
            }
        }
    } else if (state == STATE_FLASH) {
        for (size_t i = 0; i < 64; ++i)
            pageData[currentPageOffset + i] = data[i];
        currentPageOffset += 64;

        /* Flashing */
        if (currentPageOffset == sizeof(pageData)) {
            /* Received another page */
            uint32_t pageAddress = USER_PROGRAM + (currentPage * sizeof(pageData));

            /* If we're at page boundary, we have to erase this page */
            if ((pageAddress & (FLASH_MIN_ERASE_SIZE - 1)) == 0) {
#ifndef FLASH_BUFFER_WAY
                /*!< Erase this page */
                NRF_LOG_INFO("Erase start address %08x", pageAddress);
                hid_bl_erase_flash(pageAddress);
#endif
            }

            /* Then proceed to write the data */
#ifndef FLASH_BUFFER_WAY
            NRF_LOG_INFO("Write start address %08x", pageAddress);
            hid_bl_write_flash(pageAddress, pageData, sizeof(pageData));
            currentPage++;
#else
            hid_bl_write_flash_buffer(pageAddress, pageData, sizeof(pageData));
            currentPage++;
            if (currentPage == pagesToFlash) {
                /*!< Flush once */
                hid_bl_flush_flash();
            }
#endif
            currentPageOffset = 0;
        }

        /* Did we flash everything? */
        if (currentPage == pagesToFlash) {
            /* Back to processing commands */
            state = STATE_INIT;
        }
    }
}
