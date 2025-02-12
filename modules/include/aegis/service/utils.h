#ifndef AEGIS_SERVICE_UTILS_H
#define AEGIS_SERVICE_UTILS_H

/* ----------------------------------- Proto -------------------------------- */
#include <aegis_proto/aegis.grpc.pb.h>
#include <aegis_proto/aegis.pb.h>
/* ------------------------------------ Qt ---------------------------------- */
#include <QObject>
#include <QVariant>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
#include "aegis/search/query.h"
/* -------------------------------------------------------------------------- */

namespace aegis {

QVariant convertIntoVariant(const google::protobuf::Value &value);

google::protobuf::Value convertIntoValue(const QVariant &variant);

std::pair<grpc::Status, QObjectList> tryGetObjects(const ObjectQuery &query);

std::pair<grpc::Status, QObject *> tryGetSingleObject(const ObjectQuery &query);

}// namespace aegis

#endif// AEGIS_SERVICE_UTILS_H
