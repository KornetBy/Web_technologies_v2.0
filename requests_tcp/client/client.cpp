#include <iostream>
#include <winsock2.h>  // Библиотека Windows для работы с сокетами
#include <ws2tcpip.h>  // Дополнительные функции, такие как inet_pton для преобразования IP-адреса

#pragma comment(lib, "ws2_32.lib")  // Прямая ссылка на библиотеку ws2_32

#define PORT 8080

int main() {
    setlocale(LC_ALL, "RU");
    WSADATA wsaData;//структурадля создания сокета
    int iResult;

    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Ошибка инициализации Winsock: " << iResult << std::endl;
        std::cin.get();
        return 1;
    }

    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    float coordinates[2];

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Ошибка при создании сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        std::cin.get();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование адреса IPv4 из текстового в бинарный
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Неверный адрес или адрес не поддерживается\n";
        closesocket(sock);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    // Подключение к серверу
    iResult = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Ошибка подключения: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    // Ввод координат
    std::cout << "Введите координаты точки X и Y: ";
    std::cin >> coordinates[0] >> coordinates[1];

    // Отправка координат серверу
    iResult = send(sock, (char*)coordinates, sizeof(coordinates), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Ошибка при отправке данных: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        std::cin.get();
        return 1;
    }
    std::cout << "Координаты отправлены серверу\n";

    // Ожидание ответа от сервера
    char buffer[1024] = { 0 };
    iResult = recv(sock, buffer, sizeof(buffer), 0);
    if (iResult > 0) {
        std::cout << "Ответ от сервера: " << buffer << std::endl;
    }
    else if (iResult == 0) {
        std::cout << "Соединение закрыто сервером\n";
    }
    else {
        std::cerr << "Ошибка при получении ответа: " << WSAGetLastError() << std::endl;
    }

    // Закрытие сокета и завершение работы Winsock

    closesocket(sock);
    WSACleanup();
    system("pause");

    return 0;
}
