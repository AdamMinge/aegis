/* ----------------------------------- Local -------------------------------- */
#include "aegis/record/strategy.h"
/* ------------------------------------ Qt ---------------------------------- */
#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QSpinBox>
#include <QTabBar>
#include <QTextEdit>
#include <QToolBox>
#include <QToolButton>
/* -------------------------------------------------------------------------- */

namespace aegis {

/* ----------------------------------- Utils -------------------------------- */

[[nodiscard]] int findTabIndex(QTabBar *tabbar, QAbstractButton *button) {
  if (tabbar->tabsClosable()) {
    for (auto i = 0; i < tabbar->count(); ++i) {
      const auto current_button = qobject_cast<QAbstractButton *>(
          tabbar->tabButton(i, QTabBar::RightSide));

      if (button == current_button) return i;
    }
  }

  return -1;
}

/* ------------------------------- RecordStrategy --------------------------- */

RecordStrategy::RecordStrategy(int type, QObject *parent)
    : QObject(parent), m_type(type), m_widget(nullptr) {}

RecordStrategy::~RecordStrategy() = default;

void RecordStrategy::setWidget(QWidget *widget) {
  if (m_widget == widget) return;

  if (m_widget) {
    removeConnections(m_widget);
    m_widget->removeEventFilter(this);
  }

  m_widget = widget;

  if (m_widget) {
    installConnections(m_widget);
    m_widget->installEventFilter(this);
  }
}

QWidget *RecordStrategy::getWidget() const { return m_widget; }

int RecordStrategy::getType() const { return m_type; }

void RecordStrategy::installConnections(QWidget *widget) { Q_UNUSED(widget); }

void RecordStrategy::removeConnections(QWidget *widget) {
  m_widget->disconnect(this);
}

/* ---------------------------- RecordWidgetStrategy ------------------------ */

RecordWidgetStrategy::RecordWidgetStrategy(QObject *parent)
    : RecordStrategy(qMetaTypeId<QWidget>(), parent) {}

RecordWidgetStrategy::RecordWidgetStrategy(int type, QObject *parent)
    : RecordStrategy(type, parent) {}

RecordWidgetStrategy::~RecordWidgetStrategy() = default;

bool RecordWidgetStrategy::eventFilter(QObject *obj, QEvent *event) {
  if (auto widget = getWidget(); widget == obj) {
    switch (event->type()) {
      case QEvent::ContextMenu:
        onOpenContextMenu();
        break;
    }
  }

  return RecordStrategy::eventFilter(obj, event);
}

void RecordWidgetStrategy::onOpenContextMenu() {
  qDebug() << "RecordButtonStrategy::onOpenContextMenu";
}

/* ---------------------------- RecordButtonStrategy ------------------------ */

RecordButtonStrategy::RecordButtonStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QAbstractButton>(), parent) {}

RecordButtonStrategy::~RecordButtonStrategy() = default;

bool RecordButtonStrategy::eventFilter(QObject *obj, QEvent *event) {
  if (auto button = getWidgetAs<QAbstractButton>(); button == obj) {
    switch (event->type()) {
      case QEvent::MouseButtonRelease: {
        const auto mouse_event = static_cast<QMouseEvent *>(event);
        const auto mouse_position = mouse_event->position().toPoint();
        const auto button_rect = button->rect();
        if (!button_rect.contains(mouse_position)) break;

        onPressed();

        break;
      }

      case QEvent::KeyPress: {
        const auto key_event = static_cast<QKeyEvent *>(event);
        if (key_event->key() == Qt::Key_Space ||
            key_event->key() == Qt::Key_Return ||
            key_event->key() == Qt::Key_Enter) {
          onPressed();
        }

        break;
      }
    }
  }

  return RecordWidgetStrategy::eventFilter(obj, event);
}

void RecordButtonStrategy::onPressed() {
  const auto button = getWidgetAs<QAbstractButton>();

  if (button->isCheckable()) {
    onToggled(!button->isChecked());
  } else {
    onClicked();
  }
}

void RecordButtonStrategy::onClicked() {
  qDebug() << "RecordButtonStrategy::onClicked";
}

void RecordButtonStrategy::onToggled(bool checked) {
  qDebug() << "RecordButtonStrategy::onToggled = " << checked;
}

/* --------------------------- RecordComboBoxStrategy ----------------------- */

RecordComboBoxStrategy::RecordComboBoxStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QComboBox>(), parent) {}

RecordComboBoxStrategy::~RecordComboBoxStrategy() = default;

void RecordComboBoxStrategy::installConnections(QWidget *widget) {
  auto combobox = qobject_cast<QComboBox *>(widget);
  Q_ASSERT(combobox);

  connect(combobox, &QComboBox::currentIndexChanged, this,
          &RecordComboBoxStrategy::onCurrentIndexChanged);
}

void RecordComboBoxStrategy::onCurrentIndexChanged(int index) {
  qDebug() << "RecordComboBoxStrategy::onCurrentIndexChanged = " << index;
}

/* --------------------------- RecordSpinBoxStrategy ------------------------ */

RecordSpinBoxStrategy::RecordSpinBoxStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QAbstractSpinBox>(), parent) {}

RecordSpinBoxStrategy::~RecordSpinBoxStrategy() = default;

void RecordSpinBoxStrategy::installConnections(QWidget *widget) {
  if (auto spinbox = qobject_cast<QSpinBox *>(widget); spinbox) {
    connect(spinbox, &QSpinBox::valueChanged, this,
            QOverload<int>::of(&RecordSpinBoxStrategy::onValueChanged));
  } else if (auto spinbox = qobject_cast<QDoubleSpinBox *>(widget); spinbox) {
    connect(spinbox, &QDoubleSpinBox::valueChanged, this,
            QOverload<double>::of(&RecordSpinBoxStrategy::onValueChanged));
  } else {
    Q_ASSERT(false);
  }
}

void RecordSpinBoxStrategy::onValueChanged(double value) {
  qDebug() << "RecordSpinBoxStrategy::onValueChanged = " << value;
}

void RecordSpinBoxStrategy::onValueChanged(int value) {
  qDebug() << "RecordSpinBoxStrategy::onValueChanged = " << value;
}

/* ---------------------------- RecordSliderStrategy ------------------------ */

RecordSliderStrategy::RecordSliderStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QAbstractSlider>(), parent) {}

RecordSliderStrategy::~RecordSliderStrategy() = default;

void RecordSliderStrategy::installConnections(QWidget *widget) {
  auto slider = qobject_cast<QAbstractSlider *>(widget);
  Q_ASSERT(slider);

  connect(slider, &QAbstractSlider::valueChanged, this,
          &RecordSliderStrategy::onValueChanged);
}

void RecordSliderStrategy::onValueChanged(int value) {
  qDebug() << "RecordSliderStrategy::onValueChanged = " << value;
}

/* ---------------------------- RecordTabBarStrategy ------------------------ */

RecordTabBarStrategy::RecordTabBarStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QTabBar>(), parent), m_closing(false) {}

RecordTabBarStrategy::~RecordTabBarStrategy() = default;

void RecordTabBarStrategy::installConnections(QWidget *widget) {
  auto tabbar = qobject_cast<QTabBar *>(widget);
  Q_ASSERT(tabbar);

  connect(tabbar, &QTabBar::tabMoved, this, &RecordTabBarStrategy::onTabMoved);

  connect(tabbar, &QTabBar::currentChanged, this, [this](auto i) {
    if (!m_closing) onCurrentChanged(i);
    m_closing = false;
  });

  if (tabbar->tabsClosable()) {
    for (auto i = 0; i < tabbar->count(); ++i) {
      const auto button = qobject_cast<QAbstractButton *>(
          tabbar->tabButton(i, QTabBar::RightSide));

      connect(button, &QAbstractButton::released, this,
              [this, tabbar, button]() {
                m_closing = true;

                const auto i = findTabIndex(tabbar, button);
                Q_ASSERT(i >= 0);
                onTabClosed(i);
              });
    }
  }
}

void RecordTabBarStrategy::removeConnections(QWidget *widget) {
  RecordWidgetStrategy::removeConnections(widget);

  auto tabbar = qobject_cast<QTabBar *>(widget);
  Q_ASSERT(tabbar);

  if (tabbar->tabsClosable()) {
    for (auto i = 0; i < tabbar->count(); ++i) {
      const auto button = qobject_cast<QAbstractButton *>(
          tabbar->tabButton(i, QTabBar::RightSide));

      button->disconnect(this);
    }
  }
}

void RecordTabBarStrategy::onCurrentChanged(int index) {
  qDebug() << "RecordTabBarStrategy::onCurrentChanged = " << index;
}

void RecordTabBarStrategy::onTabClosed(int index) {
  qDebug() << "RecordTabBarStrategy::onTabClosed = " << index;
}

void RecordTabBarStrategy::onTabMoved(int from, int to) {
  qDebug() << "RecordTabBarStrategy::onTabMoved = (" << from << "," << to
           << ")";
}

/* ---------------------------- RecordToolBoxStrategy ----------------------- */

RecordToolBoxStrategy::RecordToolBoxStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QToolBox>(), parent) {}

RecordToolBoxStrategy::~RecordToolBoxStrategy() = default;

void RecordToolBoxStrategy::installConnections(QWidget *widget) {
  auto toolbox = qobject_cast<QToolBox *>(widget);
  Q_ASSERT(toolbox);

  connect(toolbox, &QToolBox::currentChanged, this,
          &RecordToolBoxStrategy::onCurrentChanged);
}

void RecordToolBoxStrategy::onCurrentChanged(int index) {
  qDebug() << "RecordToolBoxStrategy::onCurrentChanged = " << index;
}

/* ----------------------------- RecordMenuStrategy ------------------------- */

RecordMenuStrategy::RecordMenuStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QMenu>(), parent) {}

RecordMenuStrategy::~RecordMenuStrategy() = default;

bool RecordMenuStrategy::eventFilter(QObject *obj, QEvent *event) {
  const auto tryTrigger = [this](auto menu, auto action) {
    if (!action) return;

    const auto is_disabled = !action->isEnabled();
    const auto is_submenu = action->menu();
    const auto is_inactive = action != menu->activeAction();
    if (is_disabled || is_submenu || is_inactive) return;

    onTriggered(action);
  };

  if (auto menu = getWidgetAs<QMenu>(); menu == obj) {
    switch (event->type()) {
      case QEvent::KeyPress: {
        const auto key_event = static_cast<QKeyEvent *>(event);
        const auto key = key_event->key();

        if (key == Qt::Key_Enter || key == Qt::Key_Return) {
          const auto action = menu->activeAction();
          tryTrigger(menu, action);
        }

        break;
      }
      case QEvent::MouseButtonRelease: {
        const auto mouse_event = static_cast<QMouseEvent *>(event);
        const auto mouse_position = mouse_event->position().toPoint();
        const auto action = menu->actionAt(mouse_position);

        tryTrigger(menu, action);

        break;
      }
    }
  }

  return RecordWidgetStrategy::eventFilter(obj, event);
}

void RecordMenuStrategy::onTriggered(QAction *action) {
  qDebug() << "RecordMenuStrategy::onTriggered = " << action->text();
}

/* --------------------------- RecordTextEditStrategy ----------------------- */

RecordTextEditStrategy::RecordTextEditStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QTextEdit>(), parent) {}

RecordTextEditStrategy::~RecordTextEditStrategy() = default;

void RecordTextEditStrategy::installConnections(QWidget *widget) {
  auto textedit = qobject_cast<QTextEdit *>(widget);
  Q_ASSERT(textedit);

  connect(textedit, &QTextEdit::textChanged, this,
          &RecordTextEditStrategy::onTextChanged);
}

void RecordTextEditStrategy::onTextChanged() {
  qDebug() << "RecordTextEditStrategy::onTextChanged";
}

/* --------------------------- RecordLineEditStrategy ----------------------- */

RecordLineEditStrategy::RecordLineEditStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QLineEdit>(), parent) {}

RecordLineEditStrategy::~RecordLineEditStrategy() = default;

void RecordLineEditStrategy::installConnections(QWidget *widget) {
  auto lineedit = qobject_cast<QLineEdit *>(widget);
  Q_ASSERT(lineedit);

  connect(lineedit, &QLineEdit::textChanged, this,
          &RecordLineEditStrategy::onTextChanged);
}

bool RecordLineEditStrategy::eventFilter(QObject *obj, QEvent *event) {
  if (auto widget = getWidget(); widget == obj) {
    switch (event->type()) {
      case QEvent::KeyPress: {
        const auto key_event = static_cast<QKeyEvent *>(event);
        const auto pressed_key = key_event->key();
        if (pressed_key != Qt::Key_Return && pressed_key != Qt::Key_Enter)
          break;

        onReturnPressed();

        break;
      }
    }
  }

  return RecordWidgetStrategy::eventFilter(obj, event);
}

void RecordLineEditStrategy::onTextChanged(const QString &text) {
  qDebug() << "RecordLineEditStrategy::onTextChanged = " << text;
}

void RecordLineEditStrategy::onReturnPressed() {
  qDebug() << "RecordLineEditStrategy::onReturnPressed";
}

/* --------------------------- RecordItemViewStrategy ----------------------- */

RecordItemViewStrategy::RecordItemViewStrategy(QObject *parent)
    : RecordWidgetStrategy(qMetaTypeId<QAbstractItemView>(), parent) {}

RecordItemViewStrategy::~RecordItemViewStrategy() = default;

void RecordItemViewStrategy::installConnections(QWidget *widget) {
  auto itemview = qobject_cast<QAbstractItemView *>(widget);
  Q_ASSERT(itemview);

  auto model = itemview->model();
  if (model) {
    connect(model, &QAbstractItemModel::dataChanged, this,
            &RecordItemViewStrategy::onDataChanged);
  }
}

void RecordItemViewStrategy::removeConnections(QWidget *widget) {
  auto itemview = qobject_cast<QAbstractItemView *>(widget);
  Q_ASSERT(itemview);

  auto model = itemview->model();
  if (model) {
    model->disconnect(this);
  }
}

void RecordItemViewStrategy::onDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight,
                                           const QList<int> &roles) {
  qDebug() << "RecordItemViewStrategy::onDataChanged";
}

}  // namespace aegis
