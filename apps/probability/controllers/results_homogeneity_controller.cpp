#include "results_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/font.h>

#include "probability/app.h"
#include "probability/models/data.h"

using namespace Probability;

HomogeneityResultsView::HomogeneityResultsView(Escher::Responder * parent,
                                               Escher::SelectableTableView * table) :
    VerticalLayout(Escher::Palette::WallScreenDark),
    m_topSpacer(Palette::WallScreenDark, 0, k_topMargin),
    m_title(KDFont::SmallFont,
            I18n::Message::HomogeneityResultsTitle,
            0.5f,
            0.5f,
            Palette::GrayVeryDark,
            Escher::Palette::WallScreenDark),
    m_table(table),
    m_next(parent, I18n::Message::Next, buttonActionInvocation(), KDFont::LargeFont) {
}

void HomogeneityResultsView::buttonAction() {
}

ResultsHomogeneityController::ResultsHomogeneityController(
    StackViewController * stackViewController,
    HomogeneityStatistic * statistic) :
    Page(stackViewController),
    m_contentView(this, &m_table),
    m_tableData(&m_innerTableData),
    m_innerTableData(statistic),
    m_table(this, &m_tableData, &m_tableData, &m_tableData) {
  m_table.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_table.setDecoratorType(Escher::ScrollView::Decorator::Type::None);
  m_table.setMargins(HomogeneityResultsView::k_topMargin,
                     Metric::CommonRightMargin,
                     HomogeneityResultsView::k_topMargin,
                     Metric::CommonLeftMargin);
}

void ResultsHomogeneityController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::ResultsHomogeneity);
  Escher::Container::activeApp()->setFirstResponder(&m_table);
}

Escher::View * Probability::HomogeneityResultsView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTopSpacer:
      return &m_topSpacer;
    case k_indexOfTitle:
      return &m_title;
    case k_indexOfTable:
      return m_table;
    case k_indexOfButton:
      return &m_next;
  }
  assert(false);
  return nullptr;
}