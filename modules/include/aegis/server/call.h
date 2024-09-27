#ifndef AEGIS_SERVER_CALL_H
#define AEGIS_SERVER_CALL_H

/* ----------------------------------- GRPC --------------------------------- */
#include <grpc++/grpc++.h>
/* --------------------------------- Standard ------------------------------- */
#include <memory>
/* ----------------------------------- Local -------------------------------- */
#include "aegis/export.h"
/* -------------------------------------------------------------------------- */
#include <qdebug.h>
namespace aegis {

/* ---------------------------------- CallTag ------------------------------- */

struct LIB_AEGIS_API CallTag {
  void* callable;
};

/* ---------------------------------- Callable ------------------------------ */

class LIB_AEGIS_API Callable {
 protected:
 public:
  explicit Callable();
  virtual ~Callable();

  virtual void proceed() = 0;
};

/* ---------------------------------- CallData ------------------------------ */

template <typename SERVICE, typename REQUEST, typename RESPONSE>
class CallData : public Callable {
 protected:
  enum class CallStatus { Create, Process, Finish };

  using Request = REQUEST;
  using Response = RESPONSE;
  using Service = SERVICE;

  using ProcessResult = std::pair<grpc::Status, Response>;
  using RequestMethod = void (Service::*)(
      grpc::ServerContext*, Request*,
      grpc::ServerAsyncResponseWriter<Response>*, grpc::CompletionQueue*,
      grpc::ServerCompletionQueue*, void*);

 public:
  explicit CallData(Service* service, grpc::ServerCompletionQueue* queue,
                    CallTag tag, RequestMethod request_method);
  ~CallData() override;

  void proceed() override;

  [[nodiscard]] Service* getService() const;
  [[nodiscard]] grpc::ServerCompletionQueue* getQueue() const;

 protected:
  virtual ProcessResult process(const Request& request) const = 0;

  virtual std::unique_ptr<CallData> clone() const = 0;

 protected:
  Service* m_service;
  grpc::ServerCompletionQueue* m_queue;

  CallTag m_tag;
  RequestMethod m_request_method;
  CallStatus m_status;
  grpc::ServerContext m_context;
  Request m_request;
  grpc::ServerAsyncResponseWriter<Response> m_responder;
};

template <typename SERVICE, typename REQUEST, typename RESPONSE>
CallData<SERVICE, REQUEST, RESPONSE>::CallData(
    Service* service, grpc::ServerCompletionQueue* queue, CallTag tag,
    RequestMethod REQUEST_method)
    : m_service(service),
      m_queue(queue),
      m_tag(tag),
      m_request_method(REQUEST_method),
      m_status(CallStatus::Create),
      m_responder(&m_context) {}

template <typename SERVICE, typename REQUEST, typename RESPONSE>
CallData<SERVICE, REQUEST, RESPONSE>::~CallData() = default;

template <typename SERVICE, typename REQUEST, typename RESPONSE>
void CallData<SERVICE, REQUEST, RESPONSE>::proceed() {
  switch (m_status) {
    case CallStatus::Create: {
      m_status = CallStatus::Process;
      (m_service->*m_request_method)(&m_context, &m_request, &m_responder,
                                     m_queue, m_queue,
                                     static_cast<void*>(&m_tag));
      break;
    }
    case CallStatus::Process: {
      auto cell_data = clone().release();
      cell_data->proceed();

      const auto [status, response] = process(m_request);
      if (status.ok()) {
        m_responder.Finish(response, status, static_cast<void*>(&m_tag));
      } else {
        m_responder.FinishWithError(status, static_cast<void*>(&m_tag));
      }

      m_status = CallStatus::Finish;
      break;
    }
    default: {
      delete this;
    }
  }
}

template <typename SERVICE, typename REQUEST, typename RESPONSE>
CallData<SERVICE, REQUEST, RESPONSE>::Service*
CallData<SERVICE, REQUEST, RESPONSE>::getService() const {
  return m_service;
}

template <typename SERVICE, typename REQUEST, typename RESPONSE>
grpc::ServerCompletionQueue* CallData<SERVICE, REQUEST, RESPONSE>::getQueue()
    const {
  return m_queue;
}

}  // namespace aegis

#endif  // AEGIS_SERVER_CALL_H