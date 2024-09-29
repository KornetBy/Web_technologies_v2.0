#include <iostream>
#include <winsock2.h>  // Для Windows-сокетов
#include <ws2tcpip.h>  // Для inet_pton
#include <string>      // Для работы со строками

#pragma comment(lib, "ws2_32.lib")  // Прямая ссылка на библиотеку ws2_32

#define PORT 8080

void determine_quadrant(float x, float y, std::string& result) {
    if (x > 0 && y > 0)
        result = "Точка находится в первой четверти.\n";
    else if (x < 0 && y > 0)
        result = "Точка находится во второй четверти.\n";
    else if (x < 0 && y < 0)
        result = "Точка находится в третьей четверти.\n";
    else if (x > 0 && y < 0)
        result = "Точка находится в четвертой четверти.\n";
    else
        result = "Точка находится на оси координат.\n";
}

int main() {
    setlocale(LC_ALL, "RU");
    WSADATA wsaData;
    int iResult;

    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Ошибка инициализации Winsock: " << iResult << std::endl;
        std::cin.get();
        return 1;
    }

    SOCKET server_fd = INVALID_SOCKET;
    SOCKET new_socket = INVALID_SOCKET;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    float coordinates[2];

    // Создание сокета
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Ошибка при создании сокета: " << WSAGetLastError() << std::endl;
        WSACleanup();
        std::cin.get();
        return 1;
    }

    // Привязка сокета к порту
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Ошибка при привязке: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    // Ожидание подключения клиента
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Ошибка при прослушивании: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    std::cout << "Ожидание подключения клиента...\n";

    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket == INVALID_SOCKET) {
        std::cerr << "Ошибка при принятии подключения: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    // Получение координат от клиента
    iResult = recv(new_socket, (char*)coordinates, sizeof(coordinates), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Ошибка при получении данных: " << WSAGetLastError() << std::endl;
        closesocket(new_socket);
        closesocket(server_fd);
        WSACleanup();
        std::cin.get();
        return 1;
    }

    std::cout << "Координаты получены: X = " << coordinates[0] << ", Y = " << coordinates[1] << "\n";

    // Определение четверти и формирование ответа
    std::string result;
    determine_quadrant(coordinates[0], coordinates[1], result);

    // Отправка результата клиенту
    iResult = send(new_socket, result.c_str(), result.length(), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Ошибка при отправке ответа: " << WSAGetLastError() << std::endl;
        std::cin.get();
    }

    closesocket(new_socket);
    closesocket(server_fd);

    WSACleanup();
    system("pause");

    return 0;
}

