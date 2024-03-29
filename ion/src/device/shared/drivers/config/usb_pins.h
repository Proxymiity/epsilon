#ifndef ION_DEVICE_SHARED_CONFIG_USB_H
#define ION_DEVICE_SHARED_CONFIG_USB_H

#include <drivers/config/board.h>
#include <drivers/config/internal_flash.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace USB {
namespace Config {

using namespace Regs;

/* On the STM32F730, PA9 does not actually support alternate function 10.
 * However, because of the wiring of the USB connector on old N0110, detection
 * of when the device is plugged required the use of this undocumented setting.
 * After the revision of the USB connector and ESD protection, we can now
 * follow the specification and configure the Vbus pin as a floating-input GPIO.
 */
constexpr static AFGPIOPin VbusPin = AFGPIOPin(GPIOA, 9, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

constexpr static AFGPIOPin DmPin = AFGPIOPin(GPIOA, 11, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);
constexpr static AFGPIOPin DpPin = AFGPIOPin(GPIOA, 12, GPIO::AFR::AlternateFunction::AF10, GPIO::PUPDR::Pull::None, GPIO::OSPEEDR::OutputSpeed::Fast);

}
}
}
}

#endif
