#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <locale>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Устанавливаем локаль для поддержки русского языка
    setlocale(LC_ALL, "Russian");

    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024];
    std::string input;

    // Инициализируем Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации Winsock" << std::endl;
        return 1;
    }

    // Создаем UDP сокет
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        WSACleanup();
        return 1;
    }

    // Заполняем информацию о сервере
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    while (true) {
        // Ввод строки от пользователя
        std::cout << "Введите строку (или 'exit' для выхода): ";
        std::getline(std::cin, input);

        // Отправляем строку серверу
        sendto(sockfd, input.c_str(), input.length(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
        std::cout << "Строка отправлена на сервер." << std::endl;

        // Если введена команда выхода
        if (input == "exit") {
            std::cout << "Завершение работы клиента..." << std::endl;
            break;
        }

        // Получаем ответ от сервера
        int n = recvfrom(sockfd, buffer, 1024, 0, nullptr, nullptr);
        if (n == SOCKET_ERROR) {
            std::cerr << "Ошибка получения данных" << std::endl;
            break;
        }

        buffer[n] = '\0';
        std::cout << "Ответ от сервера (только цифры): " << buffer << std::endl;
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
