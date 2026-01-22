#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <stdexcept>

using namespace std;

class Graph {
public:
    // Основные методы
    void addEdge(int from, int to);
    // Функция принимает список списков, "таблицу" с рёбрами [[from, to], ...]
    void addEdges(const vector<vector<int>>& edges); 
    
    // Получение информации о графе
    bool hasNode(int node) const;
    vector<int> getNeighbors(int node) const;
    int getNodeCount() const;
    int getEdgeCount() const;
    
    // Проверки согласно спецификации
    bool hasMinimumSize() const; // не менее 6 вершин и 6 рёбер
    bool hasMaximumSize() const; // не более 20 вершин и 20 рёбер
    bool isEmpty() const;
    
    // Проверка существования вершин
    bool containsVertices(int start, int end) const;

private:
    // Неориентированный граф: 
    // unordered_map<int, ...> - это словарь где:
    // Ключ = номер узла (например: 1, 2, 3...)
    // Значение = список соседей
    // vector<int> - это список соседей для каждого узла
    unordered_map<int, vector<int>> graph;
};

#endif