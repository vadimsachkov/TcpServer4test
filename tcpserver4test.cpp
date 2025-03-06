#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX  // Отключаем макросы min/max из Windows API

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

std::mutex mtx;
std::atomic<int> active_connections(0);
const int MAX_CONNECTIONS = 10;

void send_time_updates(SOCKET client_socket, int connection_id, std::string client_ip, int client_port, std::atomic<bool>& user_typing) {
    int counter = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        if (user_typing.load()) continue; // Пропускаем отправку, если пользователь печатает

        std::string time_str = std::to_string(++counter) + " #" + std::to_string(connection_id) + " " + client_ip + ":" + std::to_string(client_port) + " - ";
        time_t now = time(nullptr);
        struct tm time_info;
        localtime_s(&time_info, &now);

        char time_buf[20]; // "YYYY-MM-DD hh:mm:ss" -> 19 символов + \0
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &time_info);

        time_str += std::string(time_buf) + "\r\n";

        if (send(client_socket, time_str.c_str(), time_str.size(), 0) == SOCKET_ERROR) {
            break; // Останавливаем поток при разрыве соединения
        }
    }
}

void handle_client(SOCKET client_socket, std::string client_ip, int client_port, int connection_id) {
    active_connections++;
    std::cout << "Connection #" << connection_id << " from " << client_ip << ":" << client_port << "; active connections: " << active_connections.load() << std::endl;

    const char* welcome_msg = "Welcome! Type 'q' or press ESC to exit.\r\n";
    send(client_socket, welcome_msg, strlen(welcome_msg), 0);

    std::atomic<bool> user_typing(false);
    std::thread time_thread(send_time_updates, client_socket, connection_id, client_ip, client_port, std::ref(user_typing));

    char buffer[100]; // Буфер приёма данных
    std::string received_data;

    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        user_typing.store(true);

        // Проверяем ESC - немедленный выход
        if (strchr(buffer, '\x1b') != nullptr) break;

        // Проверяем, есть ли Enter в буфере
        bool has_newline = (strchr(buffer, '\n') != nullptr);

        // Добавляем данные, но только если строка не превысила 100 символов
        if (received_data.size() < 100 && !has_newline) {
            size_t available_space = 100 - received_data.size();
            size_t bytes_to_add = (bytes_received > available_space) ? available_space : bytes_received;
            received_data.append(buffer, bytes_to_add);
        }

        // Если в буфере был Enter, начинаем обработку строки
        if (has_newline) {
            size_t first = received_data.find_first_not_of("\n");
            size_t last = received_data.find_last_not_of("\n");

            if (received_data == "q") {
                break; // Выход только если строка == "q"
            }

            // Отправляем ответ, если строка не пустая
            if (!received_data.empty()) {
                std::string response = "Received: " + received_data + "\r\n";
                send(client_socket, response.c_str(), response.size(), 0);
            }

            received_data.clear();
            user_typing.store(false);
        }
    }

    closesocket(client_socket);
    active_connections--;
    std::cout << "Closed #" << connection_id << "; active connections: " << active_connections.load() << std::endl;

    if (time_thread.joinable()) {
        time_thread.join(); // Завершаем поток обновления времени
    }
}

int main(int argc, char* argv[]) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    int port = 1088;
    if (argc > 1) port = std::atoi(argv[1]);

    WSAStartup(MAKEWORD(2, 2), &wsa);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    bind(server_socket, (struct sockaddr*)&server, sizeof(server));
    listen(server_socket, 5);

    std::cout << "Server started on port " << port << " waiting for connections..." << std::endl;

    int connection_counter = 0;
    while (true) {
        c = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr*)&client, &c);

        if (active_connections >= MAX_CONNECTIONS) {
            send(client_socket, "Connection limit exceeded. Try again later.\r\n", 44, 0);
            closesocket(client_socket);
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client.sin_port);

        connection_counter++;
        std::thread(handle_client, client_socket, std::string(client_ip), client_port, connection_counter).detach();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
