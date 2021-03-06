// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: UserService.proto

#include "UserService.pb.h"
#include "UserService.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>

static const char* UserServiceOperations_method_names[] = {
  "/UserServiceOperations/StartNotificationStream",
  "/UserServiceOperations/ProcessFile",
};

std::unique_ptr< UserServiceOperations::Stub> UserServiceOperations::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< UserServiceOperations::Stub> stub(new UserServiceOperations::Stub(channel));
  return stub;
}

UserServiceOperations::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_StartNotificationStream_(UserServiceOperations_method_names[0], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_ProcessFile_(UserServiceOperations_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::ClientReader< ::NotificationMessageDto>* UserServiceOperations::Stub::StartNotificationStreamRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request) {
  return ::grpc::internal::ClientReaderFactory< ::NotificationMessageDto>::Create(channel_.get(), rpcmethod_StartNotificationStream_, context, request);
}

void UserServiceOperations::Stub::experimental_async::StartNotificationStream(::grpc::ClientContext* context, ::google::protobuf::Empty* request, ::grpc::experimental::ClientReadReactor< ::NotificationMessageDto>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::NotificationMessageDto>::Create(stub_->channel_.get(), stub_->rpcmethod_StartNotificationStream_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::NotificationMessageDto>* UserServiceOperations::Stub::AsyncStartNotificationStreamRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::NotificationMessageDto>::Create(channel_.get(), cq, rpcmethod_StartNotificationStream_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::NotificationMessageDto>* UserServiceOperations::Stub::PrepareAsyncStartNotificationStreamRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::NotificationMessageDto>::Create(channel_.get(), cq, rpcmethod_StartNotificationStream_, context, request, false, nullptr);
}

::grpc::Status UserServiceOperations::Stub::ProcessFile(::grpc::ClientContext* context, const ::DicomFileDto& request, ::ProcessResult* response) {
  return ::grpc::internal::BlockingUnaryCall< ::DicomFileDto, ::ProcessResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_ProcessFile_, context, request, response);
}

void UserServiceOperations::Stub::experimental_async::ProcessFile(::grpc::ClientContext* context, const ::DicomFileDto* request, ::ProcessResult* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::DicomFileDto, ::ProcessResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ProcessFile_, context, request, response, std::move(f));
}

void UserServiceOperations::Stub::experimental_async::ProcessFile(::grpc::ClientContext* context, const ::DicomFileDto* request, ::ProcessResult* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_ProcessFile_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::ProcessResult>* UserServiceOperations::Stub::PrepareAsyncProcessFileRaw(::grpc::ClientContext* context, const ::DicomFileDto& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::ProcessResult, ::DicomFileDto, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_ProcessFile_, context, request);
}

::grpc::ClientAsyncResponseReader< ::ProcessResult>* UserServiceOperations::Stub::AsyncProcessFileRaw(::grpc::ClientContext* context, const ::DicomFileDto& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncProcessFileRaw(context, request, cq);
  result->StartCall();
  return result;
}

UserServiceOperations::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      UserServiceOperations_method_names[0],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< UserServiceOperations::Service, ::google::protobuf::Empty, ::NotificationMessageDto>(
          [](UserServiceOperations::Service* service,
             ::grpc::ServerContext* ctx,
             const ::google::protobuf::Empty* req,
             ::grpc::ServerWriter<::NotificationMessageDto>* writer) {
               return service->StartNotificationStream(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      UserServiceOperations_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< UserServiceOperations::Service, ::DicomFileDto, ::ProcessResult, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](UserServiceOperations::Service* service,
             ::grpc::ServerContext* ctx,
             const ::DicomFileDto* req,
             ::ProcessResult* resp) {
               return service->ProcessFile(ctx, req, resp);
             }, this)));
}

UserServiceOperations::Service::~Service() {
}

::grpc::Status UserServiceOperations::Service::StartNotificationStream(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::grpc::ServerWriter< ::NotificationMessageDto>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status UserServiceOperations::Service::ProcessFile(::grpc::ServerContext* context, const ::DicomFileDto* request, ::ProcessResult* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


