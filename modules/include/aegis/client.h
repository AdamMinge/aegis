#ifndef AEGIS_CLIENT_H
#define AEGIS_CLIENT_H

/* ------------------------------------ Qt ---------------------------------- */
#include <QTcpSocket>
#include <QThread>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

/* -------------------------------- ClientThread ---------------------------- */

class LIB_AEGIS_API ClientThread : public QThread {
  Q_OBJECT

 public:
  ClientThread(qintptr socket_descriptor, QObject* parent = nullptr);
  ~ClientThread();

 protected:
  void run() override;

 private:
  qintptr m_socket_descriptor;
};

/* ------------------------------- ClientHandler ---------------------------- */

class LIB_AEGIS_API ClientHandler : public QObject {
  Q_OBJECT

 public:
  explicit ClientHandler(qintptr socket_descriptor, QObject* parent = nullptr);
  ~ClientHandler() override;

 Q_SIGNALS:
  void error(QTcpSocket::SocketError socketError);
  void finished();

 public Q_SLOTS:
  void readyRead();
  void disconnected();

 private:
  std::unique_ptr<QTcpSocket> m_socket;
};

}  // namespace aegis

#endif  // AEGIS_CLIENT_H