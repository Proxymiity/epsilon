ion_device_userland_svc_src = $(addprefix ion/src/device/userland/, \
  drivers/authentication.cpp \
  drivers/backlight.cpp \
  drivers/battery.cpp \
  drivers/board.cpp \
  drivers/circuit_breaker.cpp \
  drivers/crc32.cpp \
  drivers/display.cpp \
  drivers/events.cpp \
  drivers/fcc_id.cpp \
  drivers/keyboard.cpp \
  drivers/led.cpp \
  drivers/pcb_version.cpp \
  drivers/persisting_bytes.cpp \
  drivers/power.cpp \
  drivers/random.cpp \
  drivers/timing.cpp \
  drivers/usb.cpp \
)

ion_device_userland_src = $(addprefix ion/src/device/userland/, \
  boot/isr.cpp \
  boot/rt0.cpp \
  drivers/clipboard.cpp \
  drivers/console_display.cpp:+consoledisplay \
  drivers/console_dummy.cpp:-consoledisplay \
  drivers/reset.cpp \
  drivers/serial_number.cpp \
  drivers/stack.cpp \
  drivers/userland_header.cpp \
)

ion_device_userland_src += ion/src/shared/collect_registers.cpp

ION_DEVICE_SFLAGS = -Iion/src/device/$(MODEL)/userland -Iion/src/device/userland
