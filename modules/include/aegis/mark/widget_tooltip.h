#ifndef AEGIS_MARK_WIDGET_TOOLTIP_H
#define AEGIS_MARK_WIDGET_TOOLTIP_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QDialog>
#include <QLabel>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------- MarkerWidgetTooltip ------------------------ */

class LIB_AEGIS_API MarkerWidgetTooltip : public QDialog {
  Q_OBJECT

 public:
  explicit MarkerWidgetTooltip(QWidget *parent = nullptr);
  ~MarkerWidgetTooltip() override;

  [[nodiscard]] QWidget *getWidget() const;
  void setWidget(QWidget *object);

 private:
  void initUi();

 private:
  QLabel *m_label;
  QWidget *m_widget;
};

}  // namespace aegis

#endif  // AEGIS_MARK_WIDGET_TOOLTIP_H