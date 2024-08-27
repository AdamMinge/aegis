#ifndef AEGIS_SNIFF_WIDGET_TOOLTIP_H
#define AEGIS_SNIFF_WIDGET_TOOLTIP_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QDialog>
#include <QLabel>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------- SnifferWidgetTooltip ------------------------ */

class LIB_AEGIS_API SnifferWidgetTooltip : public QDialog {
  Q_OBJECT

 public:
  explicit SnifferWidgetTooltip(QWidget *parent = nullptr);
  ~SnifferWidgetTooltip() override;

  [[nodiscard]] QWidget *getWidget() const;
  void setWidget(QWidget *object);

 private:
  void initUi();

 private:
  QLabel *m_label;
  QWidget *m_widget;
};

}  // namespace aegis

#endif  // AEGIS_SNIFF_WIDGET_TOOLTIP_H