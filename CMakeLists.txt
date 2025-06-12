cmake_minimum_required(VERSION 3.13)

# ---- flags before importing the SDK ----
set(PICO_DISABLE_HOST_TOOLS 1)   # <- this kills picotool & tests
set(PICO_NO_PICO_STDIO     1)
# ----------------------------------------

project(rp2040_usb)


include(pico_sdk_import.cmake)
pico_sdk_init()

add_executable(rp2040_usb
    main.c
)

pico_set_linker_script(rp2040_usb ${PICO_SDK_PATH}/src/rp2_common/pico_standard_link/memmap_default.ld)

target_compile_definitions(rp2040_usb PRIVATE
    CFG_TUSB_MCU=OPT_MCU_RP2040
    CFG_TUD_HID=1
    CFG_TUD_MSC=1
)

target_include_directories(rp2040_usb PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(rp2040_usb pico_stdlib tinyusb_device tinyusb_board tinyusb_class_hid tinyusb_class_msc)

pico_add_extra_outputs(rp2040_usb)
