#ifndef ESCHER_BUFFER_TEXT_VIEW_H
#define ESCHER_BUFFER_TEXT_VIEW_H

#include <escher/text_view.h>

namespace Escher {

class BufferTextView : public TextView {
public:
  static constexpr size_t k_maxNumberOfChar = 256;
  BufferTextView(const KDFont * font = KDFont::LargeFont,
                 float horizontalAlignment = KDContext::k_alignCenter,
                 float verticalAlignment = KDContext::k_alignCenter,
                 KDColor textColor = KDColorBlack,
                 KDColor backgroundColor = KDColorWhite,
                 size_t maxDisplayedTextLength = k_maxNumberOfChar - 1);
  void setText(const char * text) override;
  const char * text() const override;
  void appendText(const char * text);

protected:
  char m_buffer[k_maxNumberOfChar];
  size_t m_maxDisplayedTextLength;
};

}

#endif
