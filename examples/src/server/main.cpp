/* ----------------------------------- Local -------------------------------- */
#include "application.h"
#include "main_window.h"
/* -------------------------------------------------------------------------- */

/* ----------------------------------- Main --------------------------------- */

int main(int argc, char **argv) {
  Application app(argc, argv);

  MainWindow window;
  window.show();

  return app.exec();
}
