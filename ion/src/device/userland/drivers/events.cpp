#include <ion/events.h>
#include <layout_events.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Events {

void SVC_ATTRIBUTES copyTextSVC(const Event * e, const char * buffer, size_t * bufferSize) {
  SVC(SVC_EVENTS_COPY_TEXT);
}

size_t Event::copyText(char * buffer, size_t bufferSize) const {
  copyTextSVC(this, buffer, &bufferSize);
  return bufferSize;
}

void SVC_ATTRIBUTES isDefinedSVC(const Event * e, bool * res) {
  SVC(SVC_EVENTS_IS_DEFINED);
}

bool Event::isDefined() const {
  bool res;
  isDefinedSVC(this, &res);
  return res;
}

void SVC_ATTRIBUTES setShiftAlphaStatusSVC(ShiftAlphaStatus * s) {
  SVC(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS);
}

void setShiftAlphaStatus(ShiftAlphaStatus s) {
  setShiftAlphaStatusSVC(&s);
}

void SVC_ATTRIBUTES shiftAlphaStatusSVC(ShiftAlphaStatus * s) {
  SVC(SVC_EVENTS_SHIFT_ALPHA_STATUS);
}

ShiftAlphaStatus shiftAlphaStatus() {
  ShiftAlphaStatus res;
  shiftAlphaStatusSVC(&res);
  return res;
}

void SVC_ATTRIBUTES repetitionFactorSVC(int * res) {
  SVC(SVC_EVENTS_REPETITION_FACTOR);
}

int repetitionFactor() {
  int res;
  repetitionFactorSVC(&res);
  return res;
}

void SVC_ATTRIBUTES getEventSVC(int * timeout, Event * e) {
  SVC(SVC_EVENTS_GET_EVENT);
}

Event getEvent(int * timeout) {
  Event e;
  getEventSVC(timeout, &e);
  return e;
}

}
}