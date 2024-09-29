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
    struct sockaddr_in servaddr, cliaddr;
    int len, n;
    char buffer[1024];

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
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    // Привязываем сокет к адресу сервера
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки сокета" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер запущен и ожидает сообщения..." << std::endl;

    while (true) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&cliaddr, &len);
        if (n == SOCKET_ERROR) {
            std::cerr << "Ошибка получения данных" << std::endl;
            break;
        }

        buffer[n] = '\0';
        std::cout << "Получено сообщение: " << buffer << std::endl;

        // Извлекаем цифры из полученной строки
        std::string digits;
        for (int i = 0; i < n; i++) {
            if (isdigit(buffer[i])) {
                digits += buffer[i];
            }
        }

        if (digits.empty()) {
            digits = "Нет цифр в строке.";
        }

        // Отправляем только цифры обратно клиенту
        sendto(sockfd, digits.c_str(), digits.length(), 0, (struct sockaddr*)&cliaddr, len);
        std::cout << "Отправлены цифры: " << digits << std::endl;

        // Если введена команда выхода
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Завершение работы сервера..." << std::endl;
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
