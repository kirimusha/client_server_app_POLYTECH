#include "Client.h"
#include "Logger.h"
#include "Validator.h"
#include "InputParser.h"
#include <iostream>
#include <algorithm>

using namespace std;

// Выводит справку по использованию программы
void printUsage(const char* programName) {
    cout << "Использование: " << programName << " <IP-адрес> <протокол> <порт>" << endl;
    cout << endl;
    cout << "Параметры:" << endl;
    cout << "  <IP-адрес>  - IP-адрес сервера (например, 127.0.0.1)" << endl;
    cout << "  <протокол>  - Тип протокола: tcp или udp" << endl;
    cout << "  <порт>      - Номер порта сервера (1024-65535)" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  " << programName << " 127.0.0.1 tcp 8080" << endl;
    cout << "  " << programName << " 192.168.1.10 udp 12345" << endl;
}

// Обрабатывает один запрос клиента
bool processClientRequest(Client& client) {
    // 1. Ввод описания графа
    cout << "\nВведите описание графа (формат: A B, B C, C D, ...):" << endl;
    cout << "или 'exit' для завершения работы: ";
    
    string graphInput;
    getline(cin, graphInput);
    
    // Проверяем команду выхода
    if (graphInput == "exit") {
        return false;
    }
    
    // Парсим граф
    vector<InputParser::Edge> edges;
    if (!InputParser::parseGraph(graphInput, edges)) {
        Logger::error("Неверный формат ввода графа");
        return true; // Продолжаем работу
    }
    
    // Подсчитываем количество уникальных вершин
    vector<string> vertices;
    for (const auto& edge : edges) {
        // Добавляем вершины, если их ещё нет в списке
        if (find(vertices.begin(), vertices.end(), edge.vertex1) == vertices.end()) {
            vertices.push_back(edge.vertex1);
        }
        if (find(vertices.begin(), vertices.end(), edge.vertex2) == vertices.end()) {
            vertices.push_back(edge.vertex2);
        }
    }
    
    // Валидация размера графа
    string errorMessage;
    if (!Validator::isValidGraphSize(vertices.size(), edges.size(), errorMessage)) {
        Logger::error(errorMessage);
        return true;
    }
    
    // 2. Ввод начальной и конечной вершин
    cout << "Введите начальную и конечную вершины (формат: A B): ";
    string verticesInput;
    getline(cin, verticesInput);
    
    string startVertex, endVertex;
    if (!InputParser::parseVertices(verticesInput, startVertex, endVertex)) {
        Logger::error("Неверный формат вершин");
        return true;
    }
    
    // Проверяем, что вершины существуют в графе
    if (!Validator::isValidVertex(startVertex, vertices)) {
        Logger::error("Вершины не найдены в графе");
        return true;
    }
    if (!Validator::isValidVertex(endVertex, vertices)) {
        Logger::error("Вершины не найдены в графе");
        return true;
    }
    
    // 3. Формируем запрос
    Protocol::Request request;
    request.startVertex = startVertex;
    request.endVertex = endVertex;
    
    // Преобразуем рёбра в формат протокола
    for (const auto& edge : edges) {
        request.edges.push_back({edge.vertex1, edge.vertex2});
    }
    
    // 4. Отправляем запрос на сервер
    Protocol::Response response;
    if (!client.sendRequest(request, response)) {
        Logger::error("Не удалось получить ответ от сервера");
        return false; // Прекращаем работу при ошибке связи
    }
    
    // 5. Обрабатываем ответ
    if (response.success) {
        cout << "\nРезультат: " << response.pathLength << endl;
        
        // Выводим путь
        cout << "Путь: ";
        for (size_t i = 0; i < response.path.size(); i++) {
            cout << response.path[i];
            if (i < response.path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    } else {
        Logger::error(response.errorMessage);
    }
    
    return true; // Продолжаем работу
}

// Главная функция клиента
int main(int argc, char* argv[]) {
    // Проверяем количество аргументов
    // argv[0] - имя программы
    // argv[1] - IP-адрес
    // argv[2] - протокол
    // argv[3] - порт
    if (argc != 4) {
        Logger::error("Неверное количество аргументов");
        printUsage(argv[0]);
        return 1;
    }
    
    // Получаем параметры
    string serverIP = argv[1];
    string protocol = argv[2];
    
    // Приводим протокол к нижнему регистру
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);
    
    // Парсим порт
    int port;
    try {
        port = stoi(argv[3]);
    } catch (...) {
        Logger::error("Порт должен быть числом");
        printUsage(argv[0]);
        return 1;
    }
    
    // Валидация параметров
    if (!Validator::isValidIP(serverIP)) {
        Logger::error("Неверный формат IP-адреса");
        return 1;
    }
    
    if (!Validator::isValidProtocol(protocol)) {
        Logger::error("Протокол должен быть tcp или udp");
        return 1;
    }
    
    if (!Validator::isValidPort(port)) {
        Logger::error("Порт должен быть в диапазоне 1024-65535");
        return 1;
    }
    
    // Создаём и подключаем клиента
    Client client(serverIP, port, protocol);
    
    if (!client.connect()) {
        Logger::error("Не удалось подключиться к серверу");
        return 1;
    }
    
    // Главный цикл обработки запросов
    while (true) {
        try {
            if (!processClientRequest(client)) {
                // Пользователь ввёл "exit" или произошла критическая ошибка
                break;
            }
        } catch (const exception& e) {
            Logger::error(string("Ошибка: ") + e.what());
            break;
        }
    }
    
    // Отключаемся от сервера
    client.disconnect();
    Logger::info("Работа клиента завершена");
    
    return 0;
}