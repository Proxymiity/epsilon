#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_CHEVRON_H

#include <escher/message_table_cell.h>
#include <escher/chevron_view.h>

namespace Escher {

class MessageTableCellWithChevron : public MessageTableCell {
public:
  MessageTableCellWithChevron(I18n::Message message = (I18n::Message)0);
  const View * accessoryView() const override { return &m_accessoryView; }
private:
  ChevronView m_accessoryView;
};

}

#endif
