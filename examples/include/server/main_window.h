#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QMainWindow>
/* -------------------------------------------------------------------------- */

/* -------------------------------- MainWindow ------------------------------ */

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;
};

#endif  // MAIN_WINDOW_H
