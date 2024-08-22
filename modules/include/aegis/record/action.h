#ifndef AEGIS_RECORD_ACTION_H
#define AEGIS_RECORD_ACTION_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QQueue>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- RecordedAction --------------------------- */

class LIB_AEGIS_API RecordedAction {
 public:
  struct ContextMenuOpened {};
  struct ButtonClicked {};
  struct ButtonToggled {};
  struct ComboBoxCurrentChanged {};
  struct SpinBoxValueChanged {};
  struct SliderValueChanged {};
  struct TabCurrentChanged {};
  struct TabClosed {};
  struct TabMoved {};
  struct ToolBoxCurrentChanged {};
  struct ActionTriggered {};
  struct TextEditTextChanged {};
  struct LineEditTextChanged {};
  struct LineEditReturnPressed {};

 public:
  template <typename ActionSubtype>
  RecordedAction(const ActionSubtype &action);

  template <typename ActionSubtype>
  [[nodiscard]] bool is() const;

  template <typename ActionSubtype>
  [[nodiscard]] const ActionSubtype *getIf() const;

  template <typename T>
  decltype(auto) visit(T &&visitor) const;

 private:
  std::variant<ContextMenuOpened, ButtonClicked, ButtonToggled,
               ComboBoxCurrentChanged, SpinBoxValueChanged, SliderValueChanged,
               TabCurrentChanged, TabClosed, TabMoved, ToolBoxCurrentChanged,
               ActionTriggered, TextEditTextChanged, LineEditTextChanged,
               LineEditReturnPressed>
      m_data;
};

template <typename ActionSubtype>
RecordedAction::RecordedAction(const ActionSubtype &action) {
  m_data = action;
}

template <typename ActionSubtype>
bool RecordedAction::is() const {
  return std::holds_alternative<ActionSubtype>(m_data);
}

template <typename ActionSubtype>
const ActionSubtype *RecordedAction::getIf() const {
  return std::get_if<ActionSubtype>(&m_data);
}

template <typename T>
decltype(auto) RecordedAction::visit(T &&visitor) const {
  return std::visit(std::forward<T>(visitor), m_data);
}

}  // namespace aegis

#endif  // AEGIS_RECORD_ACTION_H