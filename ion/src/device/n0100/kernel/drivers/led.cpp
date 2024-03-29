#include <kernel/drivers/led.h>

namespace Ion {
namespace Device {
namespace LED {

KDColor updateColorWithPlugAndCharge() {
  /* Prevent updating LED color with charging status as the end of charge
   * current is never detected on N0100 model. */
  return getColor();
}

}
}
}
