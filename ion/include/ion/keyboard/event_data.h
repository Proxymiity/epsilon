#ifndef ION_KEYBOARD_EVENT_DATA_H
#define ION_KEYBOARD_EVENT_DATA_H

namespace Ion {
namespace Events {

class EventData {
public:
  static constexpr size_t k_maxDataSize = 8;
  static constexpr EventData Undefined() { return EventData(nullptr); }
  static constexpr EventData Textless() { return EventData(""); }
  static constexpr EventData Text(const char * text) { return EventData(text); }
  bool isDefined() const { return (m_data != nullptr); }
  const char * text() const;
private:
  constexpr EventData(const char * data) : m_data(data) {}
  const char * m_data;
};

}
}

#endif
