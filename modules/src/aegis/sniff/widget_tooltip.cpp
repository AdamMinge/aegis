/* ----------------------------------- Local -------------------------------- */
#include "aegis/sniff/widget_tooltip.h"

#include "aegis/manager.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QVBoxLayout>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ---------------------------- SnifferWidgetTooltip ------------------------ */

SnifferWidgetTooltip::SnifferWidgetTooltip(QWidget *parent)
    : QDialog(parent), m_widget(nullptr) {
  initUi();
}

SnifferWidgetTooltip::~SnifferWidgetTooltip() {}

QWidget *SnifferWidgetTooltip::getWidget() const { return m_widget; }

void SnifferWidgetTooltip::setWidget(QWidget *widget) {
  if (m_widget != widget) {
    m_widget = widget;

    if (m_widget) {
      const auto id = m_widget ? searcher()->getQuery(m_widget).toString() : "";
      m_label->setText(id);
      show();
    } else {
      hide();
    }
  }
}

void SnifferWidgetTooltip::initUi() {
  setVisible(false);
  setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
  setAttribute(Qt::WA_TranslucentBackground);
  setStyleSheet(
      "QDialog { background-color: rgba(255, 255, 224, 230); border: 1px solid "
      "gray; border-radius: 5px; }");

  m_label = new QLabel(this);
  m_label->setWordWrap(true);
  m_label->setAlignment(Qt::AlignLeft);
  m_label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  auto layout = new QVBoxLayout(this);
  layout->addWidget(m_label);
  layout->setContentsMargins(8, 8, 8, 8);

  setLayout(layout);
}

}  // namespace aegis
