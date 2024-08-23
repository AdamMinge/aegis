#ifndef APPLICATION_H
#define APPLICATION_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QApplication>
/* -------------------------------------------------------------------------- */

/* -------------------------------- Application ----------------------------- */

class Application : public QApplication {
  Q_OBJECT

 public:
  explicit Application(int &argc, char **argv);
  ~Application() override;

 private:
  void parseCommandLine();
};

#endif  // APPLICATION_H
