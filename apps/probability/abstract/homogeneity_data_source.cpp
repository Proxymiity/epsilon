#include "homogeneity_data_source.h"

#include <apps/i18n.h>
#include <string.h>

using namespace Probability;

HomogeneityTableDataSource::HomogeneityTableDataSource(TableViewDataSource * contentTable, I18n::Message headerPrefix)
    : m_contentTable(contentTable), m_topLeftCell(Escher::Palette::WallScreenDark) {
  // Headers
  constexpr int bufferSize = 20;
  char txt[bufferSize];
  const char * groupTxt = I18n::translate(headerPrefix);
  int offset = strlcpy(txt, groupTxt, bufferSize);
  offset += strlcpy(txt + offset, " ", bufferSize - offset);
  // First row
  for (int i = 0; i < k_initialNumberOfColumns; i++) {
    *(txt + offset) = '1' + i;
    m_colHeader[i].setText(txt);
    m_colHeader[i].setAlignment(.5f, .5f);
    m_colHeader[i].setFont(KDFont::SmallFont);
  }
  // First column
  for (int i = 0; i < k_initialNumberOfRows; i++) {
    *(txt + offset) = 'A' + i;
    m_rowHeader[i].setText(txt);
    m_rowHeader[i].setAlignment(.5f, .5f);
    m_rowHeader[i].setFont(KDFont::SmallFont);
    m_rowHeader[i].setEven(i % 2 == 0);
  }
}

HighlightCell * HomogeneityTableDataSource::reusableCell(int i, int type) {
  if (i == 0) {
    return &m_topLeftCell;
  }
  if (i < numberOfColumns()) {
    return &m_colHeader[i - 1];
  }
  if (i % numberOfColumns() == 0) {
    return &m_rowHeader[i / numberOfColumns() - 1];
  }
  int index = indexForEditableCell(i);
  return m_contentTable->reusableCell(index, type);
}

int HomogeneityTableDataSource::indexForEditableCell(int i) {
  // Substracts the number of cells in top and left header before this index
  int row = i / numberOfColumns();
  return i - row - numberOfColumns() + 1;
}

int HomogeneityTableDataSource::typeAtLocation(int i, int j) {
  if (i == 0 || j == 0) {
    return 0;
  }
  return m_contentTable->typeAtLocation(i - 1, j - 1);
}
