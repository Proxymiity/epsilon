#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H

#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/solid_color_view.h>
#include <escher/table_view_data_source.h>
#include <kandinsky/color.h>
#include <kandinsky/coordinate.h>

#include "homogeneity_table_view_controller.h"
#include "probability/abstract/button_delegate.h"
#include "probability/abstract/dynamic_data_source.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/abstract/input_categorical_controller.h"
#include "probability/abstract/input_categorical_view.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "results_homogeneity_controller.h"

namespace Probability {

class InputHomogeneityController : public InputCategoricalController {
public:
  InputHomogeneityController(StackViewController * parent,
                             ResultsHomogeneityController * homogeneityResultsController,
                             InputEventHandlerDelegate * inputEventHandlerDelegate,
                             HomogeneityStatistic * statistic);

  const char * title() override { return I18n::translate(I18n::Message::InputHomogeneityControllerTitle); }

  void didBecomeFirstResponder() override;

  TableViewController * tableViewController() override { return &m_tableController; }

private:
  HomogeneityTableViewController m_tableController;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H */
