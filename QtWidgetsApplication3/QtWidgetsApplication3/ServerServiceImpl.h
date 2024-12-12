#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

class ServerServiceImpl final : public MaintainingApi::Service {
public:
    grpc::Status Ping(grpc::ServerContext* context, const PingRequest* request, PingResponse* response) override {
        // ���������� ���������� ������ ��� ���� clientip
        std::string clientIp = request->clientip();  // �������� IP ������� �� �������

        // ������ ��������� �����
        std::cout << "Received Ping from: " << clientIp << std::endl;

        // ��������� �����
        return grpc::Status::OK;
    }
};