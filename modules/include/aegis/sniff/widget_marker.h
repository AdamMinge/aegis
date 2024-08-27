#ifndef AEGIS_SNIFF_WIDGET_MARKER_H
#define AEGIS_SNIFF_WIDGET_MARKER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QColor>
#include <QLabel>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- SnifferWidgetMarker ------------------------ */

class LIB_AEGIS_API SnifferWidgetMarker : public QLabel {
  Q_OBJECT

 public:
  explicit SnifferWidgetMarker(QWidget *parent = nullptr);
  ~SnifferWidgetMarker() override;

  [[nodiscard]] QWidget *getWidget() const;
  void setWidget(QWidget *widget);

  [[nodiscard]] QColor getColor() const;
  void setColor(QColor color);

 protected:
  void paintEvent(QPaintEvent *event) override;

 private:
  QWidget *m_widget;
  QColor m_color;
};

}  // namespace aegis

#endif  // AEGIS_SNIFF_WIDGET_MARKER_H