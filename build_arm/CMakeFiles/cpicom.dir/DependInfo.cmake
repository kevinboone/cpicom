# The set of languages for which implicit dependencies are needed:
set(CMAKE_DEPENDS_LANGUAGES
  "ASM"
  "C"
  "CXX"
  )
# The set of files for implicit dependencies of each language:
set(CMAKE_DEPENDS_CHECK_ASM
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_divider/divider.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_divider/divider.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/irq_handler_chain.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/irq_handler_chain.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_divider/divider.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_divider/divider.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_aeabi.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_aeabi.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_v1_rom_shim.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_v1_rom_shim.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_aeabi.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_aeabi.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_v1_rom_shim.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_v1_rom_shim.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/crt0.S" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/crt0.S.obj"
  )

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_ASM
  "CFG_TUSB_DEBUG=0"
  "CFG_TUSB_MCU=OPT_MCU_RP2040"
  "CFG_TUSB_OS=OPT_OS_PICO"
  "PICO_BIT_OPS_PICO=1"
  "PICO_BOARD=\"pico\""
  "PICO_BOOT2_NAME=\"boot2_w25q080\""
  "PICO_BUILD=1"
  "PICO_CMAKE_BUILD_TYPE=\"Release\""
  "PICO_COPY_TO_RAM=0"
  "PICO_CXX_ENABLE_EXCEPTIONS=0"
  "PICO_DIVIDER_HARDWARE=1"
  "PICO_DOUBLE_PICO=1"
  "PICO_FLOAT_PICO=1"
  "PICO_INT64_OPS_PICO=1"
  "PICO_MEM_OPS_PICO=1"
  "PICO_NO_FLASH=0"
  "PICO_NO_HARDWARE=0"
  "PICO_ON_DEVICE=1"
  "PICO_PRINTF_PICO=1"
  "PICO_STDIO_USB=1"
  "PICO_TARGET_NAME=\"cpicom\""
  "PICO_USE_BLOCKED_RAM=0"
  )

# The include file search paths:
set(CMAKE_ASM_TARGET_INCLUDE_PATH
  "../log/include"
  "../error/include"
  "../console/include"
  "../files/include"
  "../shell/include"
  "../klib/include"
  "../ymodem/include"
  "../cpm/include"
  "../picocpm/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_stdlib/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_gpio/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_base/include"
  "generated/pico_base"
  "/home/kevin/lib/pico/pico-sdk/src/boards/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_platform/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_regs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_base/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_structs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_claim/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_uart/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_time/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_timer/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_runtime/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_clocks/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_resets/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_watchdog/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_xosc/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_pll/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_vreg/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_printf/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bootrom/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_bit_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_int64_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_malloc/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_binary_info/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/include"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/common"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/hw"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_flash/include"
  )
set(CMAKE_DEPENDS_CHECK_C
  "/home/kevin/source/cpicom/console/src/console_stdio_vt100.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/console/src/console_stdio_vt100.c.obj"
  "/home/kevin/source/cpicom/cpm/src/bdos.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/cpm/src/bdos.c.obj"
  "/home/kevin/source/cpicom/cpm/src/bios.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/cpm/src/bios.c.obj"
  "/home/kevin/source/cpicom/cpm/src/intf.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/cpm/src/intf.c.obj"
  "/home/kevin/source/cpicom/cpm/src/vt.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/cpm/src/vt.c.obj"
  "/home/kevin/source/cpicom/cpm/src/z80.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/cpm/src/z80.c.obj"
  "/home/kevin/source/cpicom/error/src/error.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/error/src/error.c.obj"
  "/home/kevin/source/cpicom/files/src/compat.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/compat.c.obj"
  "/home/kevin/source/cpicom/files/src/fileblockdevice.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/fileblockdevice.c.obj"
  "/home/kevin/source/cpicom/files/src/filecontext.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/filecontext.c.obj"
  "/home/kevin/source/cpicom/files/src/filesystemlfs.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/filesystemlfs.c.obj"
  "/home/kevin/source/cpicom/files/src/filesystemmount.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/filesystemmount.c.obj"
  "/home/kevin/source/cpicom/files/src/fileutil.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/fileutil.c.obj"
  "/home/kevin/source/cpicom/files/src/flashblockdevice.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/flashblockdevice.c.obj"
  "/home/kevin/source/cpicom/files/src/lfs.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/lfs.c.obj"
  "/home/kevin/source/cpicom/files/src/lfs_util.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/files/src/lfs_util.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/audio/audio_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/audio/audio_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/cdc/cdc_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/cdc/cdc_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/dfu/dfu_rt_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/dfu/dfu_rt_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/hid/hid_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/hid/hid_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/midi/midi_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/midi/midi_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/msc/msc_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/msc/msc_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/net/net_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/net/net_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/usbtmc/usbtmc_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/usbtmc/usbtmc_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/vendor/vendor_device.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/class/vendor/vendor_device.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/common/tusb_fifo.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/common/tusb_fifo.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/device/usbd.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/device/usbd.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/device/usbd_control.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/device/usbd_control.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c.obj"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/tusb.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/tusb.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/critical_section.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/critical_section.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/lock_core.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/lock_core.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/mutex.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/mutex.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/sem.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/sem.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_time/time.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_time/time.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_time/timeout_helper.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_time/timeout_helper.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/datetime.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_util/datetime.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/pheap.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_util/pheap.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/queue.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/common/pico_util/queue.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_claim/claim.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_claim/claim.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_clocks/clocks.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_clocks/clocks.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_flash/flash.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_flash/flash.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_gpio/gpio.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_gpio/gpio.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/irq.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/irq.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_pll/pll.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_pll/pll.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_sync/sync.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_sync/sync.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_timer/timer.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_timer/timer.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_uart/uart.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_uart/uart.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_vreg/vreg.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_vreg/vreg.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_watchdog/watchdog.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_watchdog/watchdog.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_xosc/xosc.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_xosc/xosc.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bootrom/bootrom.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bootrom/bootrom.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_init_rom.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_init_rom.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_math.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/double_math.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_init_rom.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_init_rom.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_math.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/float_math.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_malloc/pico_malloc.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_malloc/pico_malloc.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_platform/platform.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_platform/platform.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_printf/printf.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_printf/printf.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_runtime/runtime.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_runtime/runtime.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/binary_info.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/binary_info.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio/stdio.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio/stdio.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/reset_interface.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb_descriptors.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/stdio_usb_descriptors.c.obj"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdlib/stdlib.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdlib/stdlib.c.obj"
  "/home/kevin/source/cpicom/klib/src/list.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/klib/src/list.c.obj"
  "/home/kevin/source/cpicom/klib/src/string.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/klib/src/string.c.obj"
  "/home/kevin/source/cpicom/log/src/log.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/log/src/log.c.obj"
  "/home/kevin/source/cpicom/main/main.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/main/main.c.obj"
  "/home/kevin/source/cpicom/picocpm/src/config.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/picocpm/src/config.c.obj"
  "/home/kevin/source/cpicom/picocpm/src/picocpm.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/picocpm/src/picocpm.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_dir.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_dir.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_dump.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_dump.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_era.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_era.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_format.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_format.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_log.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_log.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_rename.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_rename.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_setdef.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_setdef.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_stat.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_stat.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_type.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_type.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_yrecv.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_yrecv.c.obj"
  "/home/kevin/source/cpicom/shell/src/cmd_ysend.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/cmd_ysend.c.obj"
  "/home/kevin/source/cpicom/shell/src/shell.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/shell/src/shell.c.obj"
  "/home/kevin/source/cpicom/ymodem/src/ymodem.c" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/ymodem/src/ymodem.c.obj"
  )
set(CMAKE_C_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_C
  "CFG_TUSB_DEBUG=0"
  "CFG_TUSB_MCU=OPT_MCU_RP2040"
  "CFG_TUSB_OS=OPT_OS_PICO"
  "PICO_BIT_OPS_PICO=1"
  "PICO_BOARD=\"pico\""
  "PICO_BOOT2_NAME=\"boot2_w25q080\""
  "PICO_BUILD=1"
  "PICO_CMAKE_BUILD_TYPE=\"Release\""
  "PICO_COPY_TO_RAM=0"
  "PICO_CXX_ENABLE_EXCEPTIONS=0"
  "PICO_DIVIDER_HARDWARE=1"
  "PICO_DOUBLE_PICO=1"
  "PICO_FLOAT_PICO=1"
  "PICO_INT64_OPS_PICO=1"
  "PICO_MEM_OPS_PICO=1"
  "PICO_NO_FLASH=0"
  "PICO_NO_HARDWARE=0"
  "PICO_ON_DEVICE=1"
  "PICO_PRINTF_PICO=1"
  "PICO_STDIO_USB=1"
  "PICO_TARGET_NAME=\"cpicom\""
  "PICO_USE_BLOCKED_RAM=0"
  )

# The include file search paths:
set(CMAKE_C_TARGET_INCLUDE_PATH
  "../log/include"
  "../error/include"
  "../console/include"
  "../files/include"
  "../shell/include"
  "../klib/include"
  "../ymodem/include"
  "../cpm/include"
  "../picocpm/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_stdlib/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_gpio/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_base/include"
  "generated/pico_base"
  "/home/kevin/lib/pico/pico-sdk/src/boards/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_platform/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_regs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_base/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_structs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_claim/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_uart/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_time/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_timer/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_runtime/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_clocks/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_resets/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_watchdog/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_xosc/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_pll/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_vreg/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_printf/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bootrom/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_bit_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_int64_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_malloc/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_binary_info/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/include"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/common"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/hw"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_flash/include"
  )
set(CMAKE_DEPENDS_CHECK_CXX
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/new_delete.cpp" "/home/kevin/source/cpicom/build_arm/CMakeFiles/cpicom.dir/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_standard_link/new_delete.cpp.obj"
  )
set(CMAKE_CXX_COMPILER_ID "GNU")

# Preprocessor definitions for this target.
set(CMAKE_TARGET_DEFINITIONS_CXX
  "CFG_TUSB_DEBUG=0"
  "CFG_TUSB_MCU=OPT_MCU_RP2040"
  "CFG_TUSB_OS=OPT_OS_PICO"
  "PICO_BIT_OPS_PICO=1"
  "PICO_BOARD=\"pico\""
  "PICO_BOOT2_NAME=\"boot2_w25q080\""
  "PICO_BUILD=1"
  "PICO_CMAKE_BUILD_TYPE=\"Release\""
  "PICO_COPY_TO_RAM=0"
  "PICO_CXX_ENABLE_EXCEPTIONS=0"
  "PICO_DIVIDER_HARDWARE=1"
  "PICO_DOUBLE_PICO=1"
  "PICO_FLOAT_PICO=1"
  "PICO_INT64_OPS_PICO=1"
  "PICO_MEM_OPS_PICO=1"
  "PICO_NO_FLASH=0"
  "PICO_NO_HARDWARE=0"
  "PICO_ON_DEVICE=1"
  "PICO_PRINTF_PICO=1"
  "PICO_STDIO_USB=1"
  "PICO_TARGET_NAME=\"cpicom\""
  "PICO_USE_BLOCKED_RAM=0"
  )

# The include file search paths:
set(CMAKE_CXX_TARGET_INCLUDE_PATH
  "../log/include"
  "../error/include"
  "../console/include"
  "../files/include"
  "../shell/include"
  "../klib/include"
  "../ymodem/include"
  "../cpm/include"
  "../picocpm/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_stdlib/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_gpio/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_base/include"
  "generated/pico_base"
  "/home/kevin/lib/pico/pico-sdk/src/boards/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_platform/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_regs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_base/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2040/hardware_structs/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_claim/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_uart/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_time/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_timer/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_sync/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_util/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_runtime/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_clocks/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_resets/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_watchdog/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_xosc/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_pll/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_vreg/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_irq/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_printf/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_bootrom/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_bit_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_divider/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_double/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_int64_ops/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_float/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_malloc/include"
  "/home/kevin/lib/pico/pico-sdk/src/common/pico_binary_info/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_stdio_usb/include"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/src/common"
  "/home/kevin/lib/pico/pico-sdk/lib/tinyusb/hw"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include"
  "/home/kevin/lib/pico/pico-sdk/src/rp2_common/hardware_flash/include"
  )

# Targets to which this target links.
set(CMAKE_TARGET_LINKED_INFO_FILES
  )

# Fortran module output directory.
set(CMAKE_Fortran_TARGET_MODULE_DIR "")
