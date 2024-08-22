#ifndef AEGIS_RECORD_STRATEGY_H
#define AEGIS_RECORD_STRATEGY_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QPointer>
#include <QQueue>
#include <QWidget>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class RecordedAction;

/* ------------------------------- RecordStrategy --------------------------- */

class LIB_AEGIS_API RecordStrategy : public QObject {
  Q_OBJECT

 public:
  explicit RecordStrategy(int type, QObject *parent = nullptr);
  ~RecordStrategy() override;

  void setWidget(QWidget *widget);
  [[nodiscard]] QWidget *getWidget() const;

  template <typename TYPE>
  [[nodiscard]] TYPE *getWidgetAs() const;

  [[nodiscard]] int getType() const;

  [[nodiscard]] const QQueue<RecordedAction> &getRecordedActions();
  void clearRecordedActions();

 protected:
  virtual void installConnections(QWidget *widget);
  virtual void removeConnections(QWidget *widget);

  void recordAction(RecordedAction &&action);

 private:
  int m_type;
  QPointer<QWidget> m_widget;
  QQueue<RecordedAction> m_recorded_actions;
};

template <typename TYPE>
TYPE *RecordStrategy::getWidgetAs() const {
  const auto widget = getWidget();
  const auto specific_widget = qobject_cast<TYPE *>(widget);
  Q_ASSERT(widget == specific_widget);

  return specific_widget;
}

/* ---------------------------- RecordWidgetStrategy ------------------------ */

class LIB_AEGIS_API RecordWidgetStrategy : public RecordStrategy {
  Q_OBJECT

 public:
  explicit RecordWidgetStrategy(QObject *parent = nullptr);
  ~RecordWidgetStrategy() override;

 protected:
  explicit RecordWidgetStrategy(int type, QObject *parent = nullptr);

  bool eventFilter(QObject *obj, QEvent *event) override;

 private Q_SLOTS:
  void onOpenContextMenu();
};

/* ---------------------------- RecordButtonStrategy ------------------------ */

class LIB_AEGIS_API RecordButtonStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordButtonStrategy(QObject *parent = nullptr);
  ~RecordButtonStrategy() override;

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private Q_SLOTS:
  void onPressed();
  void onClicked();
  void onToggled(bool checked);
};

/* --------------------------- RecordComboBoxStrategy ----------------------- */

class LIB_AEGIS_API RecordComboBoxStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordComboBoxStrategy(QObject *parent = nullptr);
  ~RecordComboBoxStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onCurrentIndexChanged(int index);
};

/* --------------------------- RecordSpinBoxStrategy ------------------------ */

class LIB_AEGIS_API RecordSpinBoxStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordSpinBoxStrategy(QObject *parent = nullptr);
  ~RecordSpinBoxStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onValueChanged(double value);
  void onValueChanged(int value);
};

/* ---------------------------- RecordSliderStrategy ------------------------ */

class LIB_AEGIS_API RecordSliderStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordSliderStrategy(QObject *parent = nullptr);
  ~RecordSliderStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onValueChanged(int value);
};

/* ---------------------------- RecordTabBarStrategy ------------------------ */

class LIB_AEGIS_API RecordTabBarStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordTabBarStrategy(QObject *parent = nullptr);
  ~RecordTabBarStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;
  void removeConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onCurrentChanged(int index);
  void onTabClosed(int index);
  void onTabMoved(int from, int to);

 private:
  bool m_closing;
};

/* ---------------------------- RecordToolBoxStrategy ----------------------- */

class LIB_AEGIS_API RecordToolBoxStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordToolBoxStrategy(QObject *parent = nullptr);
  ~RecordToolBoxStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onCurrentChanged(int index);
};

/* ----------------------------- RecordMenuStrategy ------------------------- */

class LIB_AEGIS_API RecordMenuStrategy : public RecordStrategy {
  Q_OBJECT

 public:
  explicit RecordMenuStrategy(QObject *parent = nullptr);
  ~RecordMenuStrategy() override;

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

 private Q_SLOTS:
  void onTriggered(QAction *action);
};

/* --------------------------- RecordTextEditStrategy ----------------------- */

class LIB_AEGIS_API RecordTextEditStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordTextEditStrategy(QObject *parent = nullptr);
  ~RecordTextEditStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onTextChanged();
};

/* --------------------------- RecordLineEditStrategy ----------------------- */

class LIB_AEGIS_API RecordLineEditStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordLineEditStrategy(QObject *parent = nullptr);
  ~RecordLineEditStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

 private Q_SLOTS:
  void onTextChanged(const QString &text);
  void onReturnPressed();
};

/* --------------------------- RecordItemViewStrategy ----------------------- */

class LIB_AEGIS_API RecordItemViewStrategy : public RecordWidgetStrategy {
  Q_OBJECT

 public:
  explicit RecordItemViewStrategy(QObject *parent = nullptr);
  ~RecordItemViewStrategy() override;

 protected:
  void installConnections(QWidget *widget) override;
  void removeConnections(QWidget *widget) override;

 private Q_SLOTS:
  void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                     const QList<int> &roles);
};

}  // namespace aegis

#endif  // AEGIS_RECORD_STRATEGY_H