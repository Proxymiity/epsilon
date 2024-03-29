ANDROID_HOME ?= /usr/local/android
NDK_BUNDLE_VERSION ?= 22.1.7171670
NDK_PATH ?= $(ANDROID_HOME)/ndk/$(NDK_BUNDLE_VERSION)

ifeq ($(HOST),macos)
NDK_HOST_TAG ?= darwin-x86_64
else ifeq ($(HOST),linux)
NDK_HOST_TAG ?= linux-x86_64
endif

NDK_TOOLCHAIN_PATH = $(NDK_PATH)/toolchains/llvm/prebuilt/$(NDK_HOST_TAG)/bin

# No 64 bit device has ever shipped with an API level < 21. Consequently, there
# is no toolchain for those archs on those API levels. Let's enforce NDK_VERSION
# at 21 for these archs, and 16 for the others.

ifeq ($(ARCH),armeabi-v7a)
  NDK_TARGET = armv7a-linux-androideabi
  NDK_VERSION = 16
else ifeq ($(ARCH),arm64-v8a)
  NDK_TARGET = aarch64-linux-android
  NDK_VERSION = 21
else ifeq ($(ARCH),x86)
  NDK_TARGET = i686-linux-android
  NDK_VERSION = 16
else ifeq ($(ARCH),x86_64)
  NDK_TARGET = x86_64-linux-android
  NDK_VERSION = 21
endif

ifdef NDK_TARGET

CC = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang
CXX = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++
LD = $(NDK_TOOLCHAIN_PATH)/$(NDK_TARGET)$(NDK_VERSION)-clang++

SFLAGS += -fPIC
# If MICROPY_NLR_SETJMP is 0, the MicroPython NLR is done by
# python/src/py/nlrthumb.c and creates code containing relocations, which is not
# accepted by Android.
SFLAGS += -DMICROPY_NLR_SETJMP=1

LDFLAGS += -shared
LDFLAGS += -static-libstdc++
LDFLAGS += -Wl,-z,defs # Warn on missing symbols when linking the dynamic library

else

CC = false
CXX = false
LD = false

endif
