#include "../common/Graph.h"

// Добавить ребро в неориентированный граф (вес всегда 1)
void Graph::addEdge(int from, int to) {
    // Неориентированный граф - добавляем в обе стороны
    graph[from].push_back(to);
    graph[to].push_back(from);
    // graph[1].push_back(2);  // к узлу 1 добавляем соседа 2
    // graph[2].push_back(1);  // к узлу 2 добавляем соседа 1
    // graph = {
    //     1: [2],    ← узел 1 знает про узел 2
    //     2: [1]     ← узел 2 знает про узел 1
    // }
}

// Добавить несколько рёбер
void Graph::addEdges(const vector<vector<int>>& edges) {
    // Этот цикл автоматически перебирает все элементы контейнера от первого до последнего
    // const = не изменяем элемент (только читаем)
    // auto = компилятор сам догадается какой тип (вместо vector<int>)
    // & = работаем с оригиналом (без копирования)
    for (const auto& edge : edges) {
        if (edge.size() != 2) {
            throw invalid_argument("Ребро должно содержать 2 вершины");
        }
        addEdge(edge[0], edge[1]);
    }
}

// Проверить существование узла
bool Graph::hasNode(int node) const {
    // graph.find(node) - ищет узел в словаре
    // Если нашел возвращает указатель на узел
    // Если не нашел возвращает graph.end()
    return graph.find(node) != graph.end();
}

// Получить соседей узла
vector<int> Graph::getNeighbors(int node) const {
    if (graph.count(node)) {
        return graph.at(node); // если узел есть в графе возвращает список соседей
        // если узла нет в графе бросает исключение std::out_of_range
    }
    return {};
}

// Количество узлов
int Graph::getNodeCount() const {
    return graph.size();
}

// Количество рёбер (для неориентированного графа делим на 2)
int Graph::getEdgeCount() const {
    int count = 0;
    // Этот цикл автоматически перебирает все элементы контейнера от первого до последнего
    // const = не изменяем элемент (только читаем)
    // auto = компилятор сам догадается какой тип (вместо vector<int>)
    // & = работаем с оригиналом (без копирования)
    for (const auto& node : graph) {
        count += node.second.size(); // node.second = список соседей этого узла, .size() - размер списка
    }
    return count / 2; // так как каждое ребро учтено дважды
}

// Проверить минимальный размер (≥6 вершин и ≥6 рёбер)
bool Graph::hasMinimumSize() const {
    return getNodeCount() >= 6 && getEdgeCount() >= 6;
}

// Проверить максимальный размер (≤20 вершин и ≤20 рёбер)
bool Graph::hasMaximumSize() const {
    return getNodeCount() <= 20 && getEdgeCount() <= 20;
}

// Проверить пустой ли граф
bool Graph::isEmpty() const {
    return graph.empty();
}

// Проверить существование обеих вершин в графе
bool Graph::containsVertices(int start, int end) const {
    return hasNode(start) && hasNode(end);
}