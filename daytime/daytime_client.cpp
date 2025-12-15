#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

const int BUFFER_SIZE = 1024;
const int DAYTIME_PORT = 13;

class DaytimeClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    
public:
    DaytimeClient(const std::string& server_ip) {
        // Создание UDP сокета
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Ошибка создания сокета: " + std::string(strerror(errno)));
        }
        
        // Настройка адреса сервера
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(DAYTIME_PORT);
        
        if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
            close(sockfd);
            throw std::runtime_error("Неверный IP адрес: " + server_ip);
        }
    }
    
    ~DaytimeClient() {
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
    
    std::string getTime() {
        // Отправка пустого датаграммы (по протоколу daytime)
        if (sendto(sockfd, "", 0, 0, 
                  (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw std::runtime_error("Ошибка отправки: " + std::string(strerror(errno)));
        }
        
        // Получение ответа
        char buffer[BUFFER_SIZE];
        socklen_t addr_len = sizeof(server_addr);
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&server_addr, &addr_len);
        
        if (recv_len < 0) {
            throw std::runtime_error("Ошибка получения: " + std::string(strerror(errno)));
        }
        
        buffer[recv_len] = '\0';
        return std::string(buffer);
    }
};

int main(int argc, char* argv[]) {
    std::string server_ip = "172.16.40.1"; // Сервер кафедры
    
    if (argc > 1) {
        server_ip = argv[1];
    }
    
    try {
        DaytimeClient client(server_ip);
        std::string time = client.getTime();
        std::cout << "Текущее время: " << time;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}