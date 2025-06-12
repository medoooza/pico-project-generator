#include "bsp/board.h"
#include "tusb.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define MSC_FLASH_OFFSET (2 * 1024 * 1024 - 64 * 1024)
#define DISK_BLOCK_NUM   (64 * 1024 / 512)
#define DISK_BLOCK_SIZE  512
#define FLASH_BASE_ADDR  (XIP_BASE + MSC_FLASH_OFFSET)

void hid_task(void) {
    static uint32_t start_ms = 0;
    static bool has_key = false;

    if (board_millis() - start_ms < 1000) return;
    start_ms += 1000;

    if (tud_hid_ready()) {
        if (!has_key) {
            tud_hid_keyboard_report(0, 0, (uint8_t[]){HID_KEY_ENTER});
            has_key = true;
        } else {
            tud_hid_keyboard_report(0, 0, NULL);
            has_key = false;
        }
    }
}

int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
    memcpy(buffer, (uint8_t*)FLASH_BASE_ADDR + lba * DISK_BLOCK_SIZE + offset, bufsize);
    return bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
    uint32_t flash_off = MSC_FLASH_OFFSET + lba * DISK_BLOCK_SIZE + offset;
    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(flash_off, 4096);
    flash_range_program(flash_off, buffer, bufsize);
    restore_interrupts(ints);
    return bufsize;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) {
    memcpy(vendor_id, "RP2040  ", 8);
    memcpy(product_id, "FLASH Disk     ", 16);
    memcpy(product_rev, "1.0 ", 4);
}

bool tud_msc_test_unit_ready_cb(uint8_t lun) {
    return true;
}

int main(void) {
    board_init();
    tusb_init();

    while (1) {
        tud_task();
        hid_task();
    }
}
