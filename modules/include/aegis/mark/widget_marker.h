#ifndef AEGIS_MARK_WIDGET_MARKER_H
#define AEGIS_MARK_WIDGET_MARKER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QColor>
#include <QLabel>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------- MarkerWidgetMarker ------------------------ */

class LIB_AEGIS_API MarkerWidgetMarker : public QLabel {
  Q_OBJECT

 public:
  explicit MarkerWidgetMarker(QWidget *parent = nullptr);
  ~MarkerWidgetMarker() override;

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

#endif  // AEGIS_MARK_WIDGET_MARKER_H