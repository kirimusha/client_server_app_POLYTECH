#!/bin/bash
# Очистка тестовой среды

cleanup_test_environment() {
    echo "Очистка тестовой среды..."

    # Удаление тестовых файлов
    if [ -d "$TEST_FILES_DIR" ]; then
        rm -rf "$TEST_FILES_DIR"
        echo "  Удален каталог тестовых файлов"
    fi

    # Остановка всех серверов
    stop_all_servers

    echo -e "${GREEN}Очистка завершена${NC}"
}

stop_all_servers() {
    # Поиск и остановка серверов на тестовых портах
    for port in $(seq $BASE_PORT $((BASE_PORT + 50))); do
        if ! is_port_available $port; then
            echo "  Остановка сервера на порту $port"
            # Найдем PID процесса использующего порт
            if command -v lsof &> /dev/null; then
                lsof -ti:$port | xargs kill -9 2>/dev/null || true
            elif command -v fuser &> /dev/null; then
                fuser -k $port/tcp 2>/dev/null || true
            fi
        fi
    done

    # Дополнительная проверка
    pkill -f "$SERVER_BIN" 2>/dev/null || true
    sleep 1
}