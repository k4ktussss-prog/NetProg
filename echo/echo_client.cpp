#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>

const int BUFFER_SIZE = 1024;
const int ECHO_PORT = 7;

class EchoClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    
public:
    EchoClient(const std::string& server_ip) {
        // Создание TCP сокета
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Ошибка создания сокета: " + std::string(strerror(errno)));
        }
        
        // Настройка адреса сервера
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(ECHO_PORT);
        
        if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
            close(sockfd);
            throw std::runtime_error("Неверный IP адрес: " + server_ip);
        }
        
        // Подключение к серверу
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Ошибка подключения: " + std::string(strerror(errno)));
        }
    }
    
    ~EchoClient() {
        if (sockfd >= 0) {
            close(sockfd);
        }
    }
    
    std::string sendMessage(const std::string& message) {
        // Отправка сообщения
        ssize_t sent_bytes = send(sockfd, message.c_str(), message.length(), 0);
        if (sent_bytes < 0) {
            throw std::runtime_error("Ошибка отправки: " + std::string(strerror(errno)));
        }
        
        // Получение ответа
        char buffer[BUFFER_SIZE];
        ssize_t recv_bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (recv_bytes < 0) {
            throw std::runtime_error("Ошибка получения: " + std::string(strerror(errno)));
        }
        
        buffer[recv_bytes] = '\0';
        return std::string(buffer);
    }
};

int main(int argc, char* argv[]) {
    std::string server_ip = "172.16.40.1"; // Сервер кафедры
    
    if (argc > 1) {
        server_ip = argv[1];
    }
    
    try {
        EchoClient client(server_ip);
        
        std::string message;
        std::cout << "Введите сообщение для отправки (или 'quit' для выхода):" << std::endl;
        
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, message);
            
            if (message == "quit" || message == "exit") {
                break;
            }
            
            if (!message.empty()) {
                std::string response = client.sendMessage(message);
                std::cout << "Ответ: " << response << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}