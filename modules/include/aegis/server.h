#ifndef AEGIS_SERVER_H
#define AEGIS_SERVER_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QTcpServer>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

class ClientThread;

/* ----------------------------------- Server ------------------------------- */

class LIB_AEGIS_API Server : public QTcpServer {
  Q_OBJECT

 public:
  explicit Server(QObject* parent = nullptr);
  ~Server() override;

  void close();

 protected:
  void incomingConnection(qintptr socketDescriptor) override;

 private:
  QList<ClientThread*> m_threads;
};

}  // namespace aegis

#endif  // AEGIS_SERVER_H