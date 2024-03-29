#include <drivers/keyboard.h>
#include <drivers/power.h>
#include <drivers/trampoline.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Power {

// Public Power methods
using namespace Device::Regs;

void standbyConfiguration() {
  PWR.CR()->setPPDS(true); // Select standby when the CPU enters deepsleep
  PWR.CR()->setCSBF(true); // Clear Standby flag
  PWR.CSR()->setBRE(false); // Unable back up RAM (lower power consumption in standby)
  PWR.CSR()->setEIWUP(false); // Unable RTC (lower power consumption in standby)

  /* The pin A0 is about to be configured as a wakeup pin. However, the matrix
   * keyboard connects pin A0 (row B) with other pins (column 1, column 3...).
   * We thus shutdown this pins to avoid the potential pull-up on pin A0 due to
   * a keyboard event. For example, if the "Home" key is down, pin A0 is
   * pulled-up so enabling it as the wake up pin would trigger a wake up flag
   * instantly. */
  Device::Keyboard::shutdownGPIO();
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  PWR.CSR2()->setEWUP1(true); // Enable PA0 as wakeup pin
  PWR.CR2()->setWUPP1(false); // Define PA0 (wakeup) pin polarity (rising edge)
  PWR.CR2()->setCWUPF1(true); // Clear wakeup pin flag for PA0 (if device has already been in standby and woke up)
#endif

  CORTEX.SCR()->setSLEEPDEEP(true); // Allow Cortex-M7 deepsleep state
}

TRAMPOLINE_INTERFACE(TRAMPOLINE_SUSPEND, bootloaderSuspend, (), void, void)

}
}
}
