#include "test_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>

#include "categorical_type_controller.h"
#include "hypothesis_controller.h"
#include "probability/app.h"
#include "type_controller.h"

using namespace Probability;

TestController::TestController(Escher::StackViewController * parentResponder,
                               HypothesisController * hypothesisController,
                               TypeController * typeController,
                               CategoricalTypeController * categoricalController,
                               InputController * inputController,
                               Data::Test * globalTest,
                               Data::TestType * globalTestType,
                               Data::CategoricalType * globalCategoricalType,
                               Statistic * statistic) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfTestCells>(parentResponder),
      m_hypothesisController(hypothesisController),
      m_typeController(typeController),
      m_inputController(inputController),
      m_categoricalController(categoricalController),
      m_globalTest(globalTest),
      m_globalTestType(globalTestType),
      m_globalCategoricalType(globalCategoricalType),
      m_statistic(statistic) {
  // Create cells
  cellAtIndex(k_indexOfOneProp)->setMessage(I18n::Message::TestOneProp);
  cellAtIndex(k_indexOfOneMean)->setMessage(I18n::Message::TestOneMean);
  cellAtIndex(k_indexOfTwoProps)->setMessage(I18n::Message::TestTwoProps);
  cellAtIndex(k_indexOfTwoMeans)->setMessage(I18n::Message::TestTwoMeans);
  cellAtIndex(k_indexOfCategorical)->setMessage(I18n::Message::TestCategorical);
  cellAtIndex(k_indexOfCategorical)->setSubtitle(I18n::Message::X2Test);
}

const char * TestController::title() {
  I18n::Message title = App::app()->subapp() == Data::SubApp::Tests
                            ? I18n::Message::Tests
                            : I18n::Message::IntervalDescr;
  return I18n::translate(title);
}

void Probability::TestController::viewWillAppear() {
  Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfTestCells>::viewWillAppear();
  // Create cells whose legends vary
  I18n::Message zMessage = App::app()->subapp() == Data::SubApp::Tests ? I18n::Message::ZTest : I18n::Message::ZInterval;
  I18n::Message tOrZMessage = App::app()->subapp() == Data::SubApp::Tests ? I18n::Message::TOrZTest : I18n::Message::TOrZInterval;
  cellAtIndex(k_indexOfOneProp)->setSubtitle(zMessage);
  cellAtIndex(k_indexOfOneMean)->setSubtitle(tOrZMessage);
  cellAtIndex(k_indexOfTwoProps)->setSubtitle(zMessage);
  cellAtIndex(k_indexOfTwoMeans)->setSubtitle(tOrZMessage);
}

void TestController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Test);
  if (selectedRow() == -1 || selectedRow() >= numberOfRows()) {
    selectRow(0);
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  resetMemoization();
  m_selectableTableView.reloadData();
}

bool TestController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    Data::SubApp subapp = App::app()->subapp();
    Escher::ViewController * controller = nullptr;
    Data::Test test;
    switch (selectedRow()) {
      case k_indexOfOneProp:
        test = Data::Test::OneProp;
        if (subapp == Data::SubApp::Tests) {
          controller = m_hypothesisController;
        } else {
          controller = m_inputController;
        }
        break;
      case k_indexOfTwoProps:
        test = Data::Test::TwoProps;
        if (subapp == Data::SubApp::Tests) {
          controller = m_hypothesisController;
        } else {
          controller = m_inputController;
        }
        break;
      case k_indexOfOneMean:
        test = Data::Test::OneMean;
        controller = m_typeController;
        break;
      case k_indexOfTwoMeans:
        test = Data::Test::TwoMeans;
        controller = m_typeController;
        break;
      case k_indexOfCategorical:
        test = Data::Test::Categorical;
        controller = m_categoricalController;
        break;
    }
    assert(controller != nullptr);
    if (test != App::app()->test()) {
      if (Data::isProportion(test)) {
        *m_globalTestType = Data::TestType::ZTest;
        Statistic::initializeStatistic(m_statistic,
                                       App::app()->subapp(),
                                       test,
                                       Data::TestType::ZTest,
                                       Data::CategoricalType::Unset);
      } else {
        *m_globalTestType = Data::TestType::Unset;
      }
      *m_globalCategoricalType = Data::CategoricalType::Unset;
    }

    *m_globalTest = test;

    if (controller == m_typeController) {
      m_typeController->selectRow(0);
    } else if (controller == m_categoricalController) {
      m_categoricalController->selectRow(0);
    }

    stackOpenPage(controller, 0);
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

int TestController::numberOfRows() const {
  // Don't show Categorical cell for Interval
  return Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfTestCells>::numberOfRows() - (App::app()->subapp() == Data::SubApp::Intervals);
}
