#!/bin/bash
# Общие утилиты для тестирования

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функции вывода
print_header() {
    echo -e "\n${BLUE}==========================================${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}==========================================${NC}\n"
}

print_test_start() {
    local test_num=$1
    local total_tests=$2
    local test_name=$3
    echo -n "[$test_num/$total_tests] $test_name... "
}

print_test_result() {
    local success=$1
    local message=$2

    if [ $success -eq 1 ]; then
        echo -e "${GREEN}PASSED${NC} - $message"
    else
        echo -e "${RED}FAILED${NC} - $message"
    fi
}

# Проверка зависимостей
check_dependencies() {
    echo "Проверка зависимостей..."

    # Проверка expect
    if ! command -v expect &> /dev/null; then
        echo -e "${RED}ОШИБКА: expect не установлен${NC}"
        echo "Установите: sudo apt-get install expect"
        exit 1
    fi

    # Проверка netstat для проверки портов
    if ! command -v netstat &> /dev/null && ! command -v ss &> /dev/null; then
        echo -e "${YELLOW}ПРЕДУПРЕЖДЕНИЕ: netstat/ss не установлен${NC}"
    fi

    echo -e "${GREEN}Зависимости проверены${NC}"
}

# Проверка исполняемых файлов
check_binaries() {
    echo "Проверка исполняемых файлов..."

    if [ ! -f "$SERVER_BIN" ]; then
        echo -e "${RED}ОШИБКА: Не найден сервер: $SERVER_BIN${NC}"
        echo "Пожалуйста, скомпилируйте проект: make"
        exit 1
    fi

    if [ ! -f "$CLIENT_BIN" ]; then
        echo -e "${RED}ОШИБКА: Не найден клиент: $CLIENT_BIN${NC}"
        echo "Пожалуйста, скомпилируйте проект: make"
        exit 1
    fi

    # Проверка прав на выполнение
    if [ ! -x "$SERVER_BIN" ]; then
        chmod +x "$SERVER_BIN"
    fi

    if [ ! -x "$CLIENT_BIN" ]; then
        chmod +x "$CLIENT_BIN"
    fi

    echo -e "${GREEN}Исполняемые файлы найдены${NC}"
    echo "  Сервер: $SERVER_BIN"
    echo "  Клиент: $CLIENT_BIN"
}

# Запуск теста
run_test() {
    local test_file="$1"
    local test_name="$2"

    export CURRENT_TEST_NUM=$((CURRENT_TEST_NUM + 1))
    export TOTAL_TESTS=$((TOTAL_TESTS + 1))

    print_test_start $CURRENT_TEST_NUM "*" "$test_name"

    # Экспорт переменных для Expect скрипта
    export TEST_PORT=$((BASE_PORT + CURRENT_TEST_NUM))

    # Запуск теста
    if expect "$test_file" >> "$RESULTS_FILE" 2>&1; then
        print_test_result 1 "Успешно"
        export PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        print_test_result 0 "См. $RESULTS_FILE"
        export FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# Проверка занятости порта
is_port_available() {
    local port=$1
    if command -v netstat &> /dev/null; then
        ! netstat -tuln | grep -q ":$port "
    elif command -v ss &> /dev/null; then
        ! ss -tuln | grep -q ":$port "
    else
        # Простая проверка
        ! (echo > /dev/tcp/localhost/$port) 2>/dev/null
    fi
}

# Поиск свободного порта
find_free_port() {
    local port=$BASE_PORT
    while ! is_port_available $port; do
        port=$((port + 1))
        if [ $port -gt 65535 ]; then
            echo -e "${RED}ОШИБКА: Не удалось найти свободный порт${NC}"
            exit 1
        fi
    done
    echo $port
}