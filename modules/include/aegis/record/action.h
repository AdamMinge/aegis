#ifndef AEGIS_RECORD_ACTION_H
#define AEGIS_RECORD_ACTION_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QQueue>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/search/query.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ------------------------------- RecordedAction --------------------------- */

class LIB_AEGIS_API RecordedAction {
 public:
  struct ContextMenuOpened {
    ObjectQuery object;
  };

  struct ButtonClicked {
    ObjectQuery object;
  };

  struct ButtonToggled {
    ObjectQuery object;
    bool checked;
  };

  struct ComboBoxCurrentChanged {
    ObjectQuery object;
    int index;
  };

  struct SpinBoxValueChanged {
    ObjectQuery object;
    int value;
  };

  struct DoubleSpinBoxValueChanged {
    ObjectQuery object;
    double value;
  };

  struct SliderValueChanged {
    ObjectQuery object;
    int value;
  };

  struct TabCurrentChanged {
    ObjectQuery object;
    int index;
  };

  struct TabClosed {
    ObjectQuery object;
    int index;
  };

  struct TabMoved {
    ObjectQuery object;
    int from;
    int to;
  };

  struct ToolBoxCurrentChanged {
    ObjectQuery object;
    int index;
  };

  struct ActionTriggered {
    ObjectQuery object;
  };

  struct TextEditTextChanged {
    ObjectQuery object;
    QString value;
  };

  struct LineEditTextChanged {
    ObjectQuery object;
    QString value;
  };

  struct LineEditReturnPressed {
    ObjectQuery object;
  };

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
               ComboBoxCurrentChanged, SpinBoxValueChanged,
               DoubleSpinBoxValueChanged, SliderValueChanged, TabCurrentChanged,
               TabClosed, TabMoved, ToolBoxCurrentChanged, ActionTriggered,
               TextEditTextChanged, LineEditTextChanged, LineEditReturnPressed>
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