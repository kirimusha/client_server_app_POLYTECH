#include "Server.h"
#include "Logger.h"
#include "Validator.h"
#include "common/Dijkstra.h"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

// Размер буфера для приёма данных
const int BUFFER_SIZE = 4096;

// Конструктор сервера
Server::Server(int port, const string& protocol)
    : port(port), protocol(protocol), serverSocket(-1), isRunning(false) {
}

// Деструктор
Server::~Server() {
    stop();
}

// Запускает сервер
bool Server::start() {
    // Создаём сокет
    if (!createSocket()) {
        return false;
    }
    
    isRunning = true;
    Logger::info("Сервер запущен на порту " + to_string(port));
    return true;
}

// Останавливает сервер
void Server::stop() {
    if (!isRunning) {
        return;
    }
    
    Logger::info("Сервер завершает работу");
    isRunning = false;
    
    // Закрываем сокет
    if (serverSocket >= 0) {
        close(serverSocket);
        serverSocket = -1;
    }
    
    // Ждём завершения всех потоков клиентов
    for (auto& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    clientThreads.clear();
}

// Создаёт и настраивает сокет
bool Server::createSocket() {
    // Определяем тип сокета в зависимости от протокола
    int socketType = (protocol == "tcp") ? SOCK_STREAM : SOCK_DGRAM;
    
    // Создаём сокет
    // AF_INET - адресное семейство IPv4
    // SOCK_STREAM - TCP, SOCK_DGRAM - UDP
    serverSocket = socket(AF_INET, socketType, 0);
    if (serverSocket < 0) {
        Logger::error("Не удалось создать сокет");
        return false;
    }
    
    // Устанавливаем опцию SO_REUSEADDR, чтобы можно было переиспользовать адрес
    // Это полезно при быстром перезапуске сервера
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::warning("Не удалось установить SO_REUSEADDR");
    }
    
    // Настраиваем адрес сервера
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // Обнуляем структуру
    serverAddr.sin_family = AF_INET;            // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;    // Слушаем на всех интерфейсах
    serverAddr.sin_port = htons(port);          // Преобразуем порт в сетевой порядок байт
    
    // Привязываем сокет к адресу
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::error("Не удалось привязать сокет к порту");
        close(serverSocket);
        return false;
    }
    
    // Для TCP нужно начать слушать входящие подключения
    if (protocol == "tcp") {
        // Второй параметр - размер очереди ожидающих подключений (backlog)
        // 5 означает, что максимум 5 клиентов могут ждать подключения одновременно
        if (listen(serverSocket, 5) < 0) {
            Logger::error("Не удалось начать прослушивание");
            close(serverSocket);
            return false;
        }
    }
    
    return true;
}

// Главный цикл сервера
void Server::run() {
    if (protocol == "tcp") {
        // TCP: принимаем подключения и создаём для каждого клиента отдельный поток
        while (isRunning) {
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            // accept() блокирует выполнение до прихода нового клиента
            int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
            
            if (clientSocket < 0) {
                if (isRunning) {
                    Logger::error("Ошибка при принятии подключения");
                }
                continue;
            }
            
            Logger::info("Подключён новый клиент");
            
            // Создаём новый поток для обработки клиента
            // thread автоматически запускает функцию в отдельном потоке
            clientThreads.emplace_back(&Server::handleTCPClient, this, clientSocket);
        }
    } else {
        // UDP: один поток обрабатывает все запросы
        handleUDPClient();
    }
}

// Обрабатывает TCP-клиента
void Server::handleTCPClient(int clientSocket) {
    while (isRunning) {
        string requestData;
        
        // Получаем данные от клиента
        if (!receiveTCP(clientSocket, requestData)) {
            // Клиент отключился или произошла ошибка
            break;
        }
        
        // Десериализуем запрос
        Protocol::Request request;
        if (!Protocol::deserializeRequest(requestData, request)) {
            Logger::error("Ошибка десериализации запроса");
            continue;
        }
        
        // Обрабатываем запрос
        Protocol::Response response;
        processRequest(request, response);
        
        // Сериализуем ответ
        string responseData = Protocol::serializeResponse(response);
        
        // Отправляем ответ клиенту
        if (!sendTCP(clientSocket, responseData)) {
            break;
        }
    }
    
    // Закрываем соединение с клиентом
    close(clientSocket);
    Logger::info("Клиент отключён");
}

// Обрабатывает UDP-клиентов
void Server::handleUDPClient() {
    while (isRunning) {
        string requestData;
        sockaddr_in clientAddr;
        
        // Получаем датаграмму
        if (!receiveUDP(requestData, clientAddr)) {
            continue;
        }
        
        // Десериализуем запрос
        Protocol::Request request;
        if (!Protocol::deserializeRequest(requestData, request)) {
            Logger::error("Ошибка десериализации запроса");
            continue;
        }
        
        Logger::info("Получен запрос от UDP-клиента");
        
        // Обрабатываем запрос
        Protocol::Response response;
        processRequest(request, response);
        
        // Сериализуем ответ
        string responseData = Protocol::serializeResponse(response);
        
        // Отправляем ответ
        sendUDP(responseData, clientAddr);
    }
}

// Обрабатывает запрос клиента
void Server::processRequest(const Protocol::Request& request, 
                            Protocol::Response& response) {
    // Создаём граф из данных запроса
    Graph graph;
    
    // Добавляем все рёбра в граф
    for (const auto& edge : request.edges) {
        // Вес всегда равен 1 (по требованиям)
        graph.addEdge(edge.first, edge.second, 1);
    }
    
    // Проверяем, что начальная и конечная вершины существуют
    vector<string> allVertices = graph.getVertices();
    if (!Validator::isValidVertex(request.startVertex, allVertices)) {
        response.success = false;
        response.errorMessage = "Вершины не найдены в графе";
        return;
    }
    if (!Validator::isValidVertex(request.endVertex, allVertices)) {
        response.success = false;
        response.errorMessage = "Вершины не найдены в графе";
        return;
    }
    
    // Запускаем алгоритм Дейкстры
    vector<string> path;
    int distance = Dijkstra::findShortestPath(graph, request.startVertex, 
                                              request.endVertex, path);
    
    // Проверяем результат
    if (distance == -1) {
        // Путь не найден
        response.success = false;
        response.errorMessage = "Путь между вершинами не существует";
    } else {
        // Путь найден
        response.success = true;
        response.pathLength = distance;
        response.path = path;
    }
}

// Отправляет данные по TCP
bool Server::sendTCP(int socket, const string& data) {
    // Сначала отправляем размер данных (4 байта)
    uint32_t dataSize = data.size();
    uint32_t networkSize = htonl(dataSize); // Преобразуем в сетевой порядок байт
    
    if (send(socket, &networkSize, sizeof(networkSize), 0) < 0) {
        return false;
    }
    
    // Затем отправляем сами данные
    if (send(socket, data.c_str(), data.size(), 0) < 0) {
        return false;
    }
    
    return true;
}

// Получает данные по TCP
bool Server::receiveTCP(int socket, string& data) {
    // Сначала читаем размер данных
    uint32_t networkSize;
    int bytesRead = recv(socket, &networkSize, sizeof(networkSize), 0);
    
    if (bytesRead <= 0) {
        // Соединение закрыто или ошибка
        return false;
    }
    
    uint32_t dataSize = ntohl(networkSize); // Преобразуем из сетевого порядка
    
    // Проверяем разумность размера
    if (dataSize > BUFFER_SIZE) {
        Logger::error("Слишком большой размер данных");
        return false;
    }
    
    // Читаем данные
    char buffer[BUFFER_SIZE];
    bytesRead = recv(socket, buffer, dataSize, 0);
    
    if (bytesRead <= 0) {
        return false;
    }
    
    data.assign(buffer, bytesRead);
    return true;
}

// Отправляет данные по UDP
bool Server::sendUDP(const string& data, const sockaddr_in& clientAddr) {
    int bytesSent = sendto(serverSocket, data.c_str(), data.size(), 0,
                          (sockaddr*)&clientAddr, sizeof(clientAddr));
    return bytesSent > 0;
}

// Получает данные по UDP
bool Server::receiveUDP(string& data, sockaddr_in& clientAddr) {
    char buffer[BUFFER_SIZE];
    socklen_t addrLen = sizeof(clientAddr);
    
    int bytesRead = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                            (sockaddr*)&clientAddr, &addrLen);
    
    if (bytesRead <= 0) {
        return false;
    }
    
    data.assign(buffer, bytesRead);
    return true;
}