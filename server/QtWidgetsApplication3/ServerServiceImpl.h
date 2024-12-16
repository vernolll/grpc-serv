#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"
#include <functional>

class ServerServiceImpl final : public MaintainingApi::Service {
public:
    using Logger = std::function<void(const std::string&)>;

    void setLogger(Logger logger);

    grpc::Status Ping(grpc::ServerContext* context, const PingRequest* request, PingResponse* response) override;

private:
    Logger logger;
};
