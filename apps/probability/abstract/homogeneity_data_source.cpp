#include "homogeneity_data_source.h"

#include <apps/i18n.h>
#include <poincare/print.h>
#include <string.h>

using namespace Probability;

HomogeneityTableDataSource::HomogeneityTableDataSource(Escher::SelectableTableViewDelegate * tableDelegate, DynamicCellsDataSourceDelegate<EvenOddBufferTextCell> * dynamicDataSourceDelegate) :
  DynamicCellsDataSource<EvenOddBufferTextCell, k_homogeneityTableNumberOfReusableHeaderCells>(dynamicDataSourceDelegate),
  ChainedSelectableTableViewDelegate(tableDelegate),
  m_headerPrefix(I18n::Message::Group),
  m_topLeftCell(Escher::Palette::WallScreenDark) {
}

int HomogeneityTableDataSource::reusableCellCount(int type) {
  if (type == k_typeOfTopLeftCell) {
    return 1;
  } else if (type == k_typeOfHeaderCells) {
    return k_numberOfReusableRows + k_numberOfReusableColumns;
  }
  return k_numberOfReusableCells;
}

HighlightCell * HomogeneityTableDataSource::reusableCell(int i, int type) {
  if (type == k_typeOfTopLeftCell) {
    assert(i == 0);
    return &m_topLeftCell;
  } else if (type == k_typeOfHeaderCells) {
    return cell(i);
  }
  return innerCell(i);
}

int HomogeneityTableDataSource::typeAtLocation(int i, int j) {
  if (i == 0 && j == 0) {
    return k_typeOfTopLeftCell;
  }
  if (i== 0 || j == 0) {
    return k_typeOfHeaderCells;
  }
  return k_typeInnerCells;
}

void Probability::HomogeneityTableDataSource::willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) {
  if (row == 0 && column == 0) {
    return;  // Top left
  }
  // Headers
  if (row == 0 || column == 0) {
    Escher::EvenOddBufferTextCell * myCell = static_cast<Escher::EvenOddBufferTextCell *>(cell);
    char digit;
    if (row == 0) {
      myCell->setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
      myCell->setFont(KDFont::SmallFont);
      myCell->setEven(true);
      assert(column - 1 <= '9' - '1');
      digit = '1' + (column - 1);
    } else {
      myCell->setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
      myCell->setFont(KDFont::SmallFont);
      myCell->setEven(row % 2 == 0);
      assert(row - 1 <= 'Z' - 'A');
      digit = 'A' + (row - 1);
    }
    constexpr int bufferSize = k_headerTranslationBuffer;
    char txt[bufferSize];
    Poincare::Print::customPrintf(txt, bufferSize, "%s%c", I18n::translate(m_headerPrefix), digit);
    myCell->setText(txt);
    KDColor textColor = KDColorBlack;
    if ((row == 0 && column == numberOfColumns() - 1 && column < k_maxNumberOfColumns) ||
        (column == 0 && row == numberOfRows() - 1 && row < k_maxNumberOfRows)) {
      textColor = Palette::GrayDark;
    }
    // TODO : Color the last max column or row gray if it is empty
    myCell->setTextColor(textColor);
  }

  else {
    willDisplayInnerCellAtLocation(cell, column - 1, row - 1);
  }
}

void Probability::HomogeneityTableDataSource::tableViewDidChangeSelection(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Prevent top left selection
  if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
    t->selectRow(previousSelectedCellY);
    t->selectColumn(previousSelectedCellX);
  }
}
