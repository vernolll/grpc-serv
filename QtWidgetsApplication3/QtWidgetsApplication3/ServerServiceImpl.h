#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

class ServerServiceImpl final : public MaintainingApi::Service {
public:
    grpc::Status Ping(grpc::ServerContext* context, const PingRequest* request, PingResponse* response) override {
        // Используем правильный геттер для поля clientip
        std::string clientIp = request->clientip();  // Получаем IP клиента из запроса

        // Логика обработки пинга
        std::cout << "Received Ping from: " << clientIp << std::endl;

        // Заполняем ответ
        return grpc::Status::OK;
    }
};