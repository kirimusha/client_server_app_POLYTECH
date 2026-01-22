#!/bin/bash
# Конфигурация тестов для CMake сборки

# Получаем директорию скрипта
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Определяем корневую директорию (build/tests -> build -> source)
BUILD_DIR="$(dirname "$SCRIPT_DIR")"
PROJECT_ROOT="$(dirname "$BUILD_DIR")"

# Пути к исполняемым файлам (CMake собирает в bin/)
export SERVER_BIN="$BUILD_DIR/bin/server"
export CLIENT_BIN="$BUILD_DIR/bin/client"

# Альтернативные пути (если CMake создает в другой структуре)
if [ ! -f "$SERVER_BIN" ]; then
    export SERVER_BIN="$BUILD_DIR/server"
fi

if [ ! -f "$CLIENT_BIN" ]; then
    export CLIENT_BIN="$BUILD_DIR/client"
fi

# Проверяем, существуют ли файлы
check_binaries() {
    if [ ! -f "$SERVER_BIN" ]; then
        echo "❌ ERROR: Server binary not found: $SERVER_BIN"
        echo "   Please build the project first:"
        echo "   cd $PROJECT_ROOT && mkdir -p build && cd build && cmake .. && make"
        exit 1
    fi

    if [ ! -f "$CLIENT_BIN" ]; then
        echo "❌ ERROR: Client binary not found: $CLIENT_BIN"
        echo "   Please build the project first:"
        echo "   cd $PROJECT_ROOT && mkdir -p build && cd build && cmake .. && make"
        exit 1
    fi

    echo "✅ Binaries found:"
    echo "   Server: $SERVER_BIN"
    echo "   Client: $CLIENT_BIN"
}

# Параметры тестов
export TEST_TIMEOUT=15
export SERVER_START_TIMEOUT=3

# Портовая база
export BASE_PORT=18080

# Каталоги тестов
export TEST_DIR="$SCRIPT_DIR"
export PROTOCOLS_DIR="$TEST_DIR/protocols"
export INPUT_METHODS_DIR="$TEST_DIR/input_methods"
export VALIDATION_DIR="$TEST_DIR/validation"
export ALGORITHMS_DIR="$TEST_DIR/algorithms"

# Тестовые файлы
export TEST_FILES_DIR="$TEST_DIR/test_files"
mkdir -p "$TEST_FILES_DIR"
export TEST_GRAPH_5V="$TEST_FILES_DIR/test_graph_5v.txt"
export TEST_GRAPH_6V="$TEST_FILES_DIR/test_graph_6v.txt"
export TEST_GRAPH_13V="$TEST_FILES_DIR/test_graph_13v.txt"
export TEST_GRAPH_20V="$TEST_FILES_DIR/test_graph_20v.txt"
export TEST_GRAPH_21V="$TEST_FILES_DIR/test_graph_21v.txt"

# Результаты тестов
export TIMESTAMP=$(date +%Y%m%d_%H%M%S)
export RESULTS_FILE="$TEST_DIR/results_$TIMESTAMP.log"
export SUMMARY_FILE="$TEST_DIR/summary_$TIMESTAMP.txt"

# Счетчики тестов
export TOTAL_TESTS=0
export PASSED_TESTS=0
export FAILED_TESTS=0
export CURRENT_TEST_NUM=0
export CURRENT_TEST_NAME=""