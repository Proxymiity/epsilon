ifeq ($(FIRMWARE_COMPONENT),bootloader)
include ion/src/$(PLATFORM)/$(MODEL)/epsilon-bootloader/$(FIRMWARE_COMPONENT)/Makefile
else
-include ion/src/$(PLATFORM)/$(MODEL)/$(FIRMWARE_COMPONENT)/Makefile
endif

include ion/src/$(PLATFORM)/$(MODEL)/shared/Makefile

# We need to work around a GCC bug (concerning versions < 5.1). It is valid in
# C++11 to initialize a character array by providing a string litteral (e.g.
# char test[4]= "ab"; is valid and should initialize test to 'a','b',0,0).
# Older versions of GCC are not conformant so we resort to an initializer list.
initializer_list = $(shell echo $(1) | sed  "s/\(.\)/'\1',/g")0
$(call object_for,ion/src/$(PLATFORM)/kernel/drivers/kernel_header.cpp ion/src/$(PLATFORM)/userland/drivers/userland_header.cpp): SFLAGS += -DPATCH_LEVEL="$(call initializer_list,$(PATCH_LEVEL))" -DEPSILON_VERSION="$(call initializer_list,$(EPSILON_VERSION))"

ION_DEVICE_SFLAGS += -Iion/src/$(PLATFORM)/$(MODEL)/shared -Iion/src/$(PLATFORM)/$(MODEL) -Iion/src/$(PLATFORM)/shared -Iion/src/$(PLATFORM) #TODO EMILIE: remove */shared and specify shared in inclusions

IN_FACTORY ?= 0
ION_DEVICE_SFLAGS += -DPCB_LATEST=$(PCB_LATEST) -DIN_FACTORY=$(IN_FACTORY)

$(call object_for,$(ion_$(PLATFORM)_$(FIRMWARE_COMPONENT)_src)): SFLAGS += $(ION_DEVICE_SFLAGS)

