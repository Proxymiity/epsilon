#ifndef APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H
#define APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H

#include <escher/list_view_data_source.h>

#include "highlight_image_cell.h"
#include "probability/models/distribution/distribution.h"

namespace Probability {

/* Provides the views shown to select the desired calculation. */
class CalculationPopupDataSource : public Escher::ListViewDataSource {
public:
  CalculationPopupDataSource(Distribution * distribution) : m_distribution(distribution) {}
  int numberOfRows() const override;
  KDCoordinate rowHeight(int r) override { return 1; /* TODO useless? */ }
  int reusableCellCount(int type) override { return k_numberOfImages; }
  Escher::HighlightCell * reusableCell(int i, int type) override { return &m_imageCells[i]; }
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  constexpr static int k_numberOfImages = 4;

private:
  HighlightImageCell m_imageCells[k_numberOfImages];
  Distribution * m_distribution;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_CALCULATION_POPUP_DATA_SOURCE_H */