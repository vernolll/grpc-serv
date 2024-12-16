#include "ServerServiceImpl.h"

void ServerServiceImpl::setLogger(Logger logger) {
    this->logger = logger;
}

grpc::Status ServerServiceImpl::Ping(grpc::ServerContext* context, const PingRequest* request, PingResponse* response) {
    std::string clientIp = request->clientip();

    if (logger) {
        logger("Received Ping from client IP: " + clientIp);
    }

    if (logger) {
        logger("Sending Pong response to client: " + clientIp);
    }

    return grpc::Status::OK;  // Empty answer
}