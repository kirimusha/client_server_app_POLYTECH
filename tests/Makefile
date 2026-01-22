# Makefile для сборки проекта и запуска тестов

# Определяем корневую директорию проекта
PROJECT_ROOT := $(shell pwd)
BUILD_DIR = $(PROJECT_ROOT)/build
TESTS_DIR = $(PROJECT_ROOT)/tests
COMMON_DIR = $(PROJECT_ROOT)/common
UTILS_DIR = $(PROJECT_ROOT)/utils
SERVER_DIR = $(PROJECT_ROOT)/server
CLIENT_DIR = $(PROJECT_ROOT)/client

# Исходные файлы
SERVER_SOURCES = \
	$(SERVER_DIR)/Server.cpp \
	$(SERVER_DIR)/ServerMain.cpp \
	$(COMMON_DIR)/Graph.cpp \
	$(COMMON_DIR)/Protocol.cpp \
	$(COMMON_DIR)/UDPProtocol.cpp \
	$(COMMON_DIR)/Dijkstra.cpp \
	$(UTILS_DIR)/FileReader.cpp \
	$(UTILS_DIR)/InputParser.cpp \
	$(UTILS_DIR)/Validator.cpp \
	$(UTILS_DIR)/Logger.cpp

CLIENT_SOURCES = \
	$(CLIENT_DIR)/Client.cpp \
	$(CLIENT_DIR)/ClientMain.cpp \
	$(COMMON_DIR)/Graph.cpp \
	$(COMMON_DIR)/Protocol.cpp \
	$(COMMON_DIR)/UDPProtocol.cpp \
	$(COMMON_DIR)/Dijkstra.cpp \
	$(UTILS_DIR)/FileReader.cpp \
	$(UTILS_DIR)/InputParser.cpp \
	$(UTILS_DIR)/Validator.cpp \
	$(UTILS_DIR)/Logger.cpp

# Исполняемые файлы
SERVER_BIN = $(BUILD_DIR)/server
CLIENT_BIN = $(BUILD_DIR)/client

BINARIES = $(SERVER_BIN) $(CLIENT_BIN)

# Компилятор
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I$(COMMON_DIR) -I$(UTILS_DIR) -I$(SERVER_DIR) -I$(CLIENT_DIR)
LDFLAGS = -pthread -lm

# Цвета для вывода
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
NC = \033[0m

.PHONY: all build test clean help run-server run-client kill-server check-structure

all: build test

# Сборка проекта
build: $(BINARIES)

$(SERVER_BIN): $(SERVER_SOURCES)
	@echo "$(BLUE)🔨 Компиляция сервера...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(SERVER_SOURCES) -o $@ $(LDFLAGS)
	@echo "$(GREEN)✅ Сервер скомпилирован: $@$(NC)"

$(CLIENT_BIN): $(CLIENT_SOURCES)
	@echo "$(BLUE)🔨 Компиляция клиента...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) -o $@ $(LDFLAGS)
	@echo "$(GREEN)✅ Клиент скомпилирован: $@$(NC)"

# Проверка структуры проекта
check-structure:
	@echo "$(CYAN)🔍 Проверка структуры проекта...$(NC)"
	@echo "Корень проекта: $(PROJECT_ROOT)"
	@echo "Текущая директория: $(shell pwd)"
	@test -d $(COMMON_DIR) || (echo "$(RED)❌ Папка common не найдена: $(COMMON_DIR)$(NC)" && exit 1)
	@test -d $(UTILS_DIR) || (echo "$(RED)❌ Папка utils не найдена: $(UTILS_DIR)$(NC)" && exit 1)
	@test -d $(SERVER_DIR) || (echo "$(RED)❌ Папка server не найдена: $(SERVER_DIR)$(NC)" && exit 1)
	@test -d $(CLIENT_DIR) || (echo "$(RED)❌ Папка client не найдена: $(CLIENT_DIR)$(NC)" && exit 1)
	@echo "$(GREEN)✅ Структура проекта корректна$(NC)"

# Быстрый вариант проверки
check:
	@echo "$(CYAN)🔍 Быстрая проверка структуры...$(NC)"
	@if [ -d ../common ]; then \
		echo "$(GREEN)✅ Папка common найдена$(NC)"; \
	else \
		echo "$(RED)❌ Папка common не найдена$(NC)"; \
		exit 1; \
	fi
	@if [ -d ../server ]; then \
		echo "$(GREEN)✅ Папка server найдена$(NC)"; \
	else \
		echo "$(RED)❌ Папка server не найдена$(NC)"; \
		exit 1; \
	fi

# Проверка исходных файлов
check-sources:
	@echo "$(CYAN)📁 Проверка исходных файлов...$(NC)"
	@for file in $(SERVER_SOURCES); do \
		if [ -f $$file ]; then \
			echo "  ✅ $$file"; \
		else \
			echo "$(YELLOW)  ⚠️  Файл $$file не найден$(NC)"; \
		fi \
	done
	@echo ""
	@for file in $(CLIENT_SOURCES); do \
		if [ -f $$file ]; then \
			echo "  ✅ $$file"; \
		else \
			echo "$(YELLOW)  ⚠️  Файл $$file не найден$(NC)"; \
		fi \
	done
	@echo "$(GREEN)✅ Проверка файлов завершена$(NC)"

# Подготовка к сборке
prepare: check-structure check-sources
	@echo "$(GREEN)🚀 Готов к сборке!$(NC)"

# Запуск тестов
test: build
	@echo "$(PURPLE)🧪 Запуск автоматических тестов...$(NC)"
	@cd $(TESTS_DIR) && ./run_tests.sh

# Запуск определенной группы тестов
test-protocols: build
	@echo "$(PURPLE)🧪 Запуск тестов протоколов...$(NC)"
	@cd $(TESTS_DIR) && bash -c 'source utils.sh && print_section "ТЕСТЫ ПРОТОКОЛОВ"'
	@cd $(TESTS_DIR) && for test in protocols/*.expect; do \
		echo "Запуск $$test..."; \
		expect $$test && echo "$(GREEN)✓ PASSED$${NC}" || echo "$(RED)✗ FAILED$${NC}"; \
	done

test-validation: build
	@echo "$(PURPLE)🧪 Запуск тестов валидации...$(NC)"
	@cd $(TESTS_DIR) && bash -c 'source utils.sh && print_section "ТЕСТЫ ВАЛИДАЦИИ"'
	@cd $(TESTS_DIR) && for test in validation/*.expect; do \
		echo "Запуск $$test..."; \
		expect $$test && echo "$(GREEN)✓ PASSED$${NC}" || echo "$(RED)✗ FAILED$${NC}"; \
	done

test-input: build
	@echo "$(PURPLE)🧪 Запуск тестов ввода...$(NC)"
	@cd $(TESTS_DIR) && bash -c 'source utils.sh && print_section "ТЕСТЫ ВВОДА"'
	@cd $(TESTS_DIR) && for test in input_methods/*.expect; do \
		echo "Запуск $$test..."; \
		expect $$test && echo "$(GREEN)✓ PASSED$${NC}" || echo "$(RED)✗ FAILED$${NC}"; \
	done

# Быстрый тест (только основные сценарии)
quick-test: build
	@echo "$(PURPLE)🧪 Запуск быстрых тестов...$(NC)"
	@cd $(TESTS_DIR) && \
		expect protocols/test_tcp_basic.expect && \
		expect protocols/test_udp_basic.expect && \
		expect validation/test_graph_at_min.expect && \
		expect validation/test_graph_at_max.expect

# Запуск сервера TCP
run-server-tcp: build
	@echo "$(BLUE)🚀 Запуск сервера (TCP, порт 8080)...$(NC)"
	@$(SERVER_BIN) 8080 tcp

# Запуск сервера UDP
run-server-udp: build
	@echo "$(BLUE)🚀 Запуск сервера (UDP, порт 12345)...$(NC)"
	@$(SERVER_BIN) 12345 udp

# Запуск клиента TCP
run-client-tcp: build
	@echo "$(CYAN)🚀 Запуск клиента (TCP, localhost:8080)...$(NC)"
	@$(CLIENT_BIN) 127.0.0.1 tcp 8080

# Запуск клиента UDP
run-client-udp: build
	@echo "$(CYAN)🚀 Запуск клиента (UDP, localhost:12345)...$(NC)"
	@$(CLIENT_BIN) 127.0.0.1 udp 12345

# Убить все серверы
kill-server:
	@echo "$(YELLOW)🛑 Остановка всех серверов...$(NC)"
	@pkill -f "$(SERVER_BIN)" 2>/dev/null || true
	@sleep 1
	@echo "$(GREEN)✅ Все серверы остановлены$(NC)"

# Отладочная сборка
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: clean build
	@echo "$(GREEN)🔧 Отладочная сборка завершена$(NC)"

# Релизная сборка
release: CXXFLAGS += -O3 -DNDEBUG
release: clean build
	@echo "$(GREEN)🚀 Релизная сборка завершена$(NC)"

# Очистка
clean:
	@echo "$(YELLOW)🧹 Очистка...$(NC)"
	@rm -rf $(BUILD_DIR)
	@cd $(TESTS_DIR) && rm -rf test_files results_*.log summary_*.txt 2>/dev/null || true
	@echo "$(GREEN)✅ Очистка завершена$(NC)"

# Показать последний отчет
report:
	@echo "$(BLUE)📊 Последний отчет тестирования:$(NC)"
	@ls -t $(TESTS_DIR)/summary_*.txt 2>/dev/null | head -1 | xargs cat 2>/dev/null || echo "$(YELLOW)📭 Отчеты не найдены$(NC)"

# Показать структуру проекта
ls:
	@echo "$(BLUE)📁 Структура проекта:$(NC)"
	@find . -maxdepth 3 -type d | sort | sed 's|^\./||'

# Справка
help:
	@echo "$(BLUE)📖 ДОСТУПНЫЕ КОМАНДЫ:$(NC)"
	@echo ""
	@echo "$(GREEN)📦 Сборка:$(NC)"
	@echo "  make               - Собрать проект и запустить тесты"
	@echo "  make build         - Только сборка проекта"
	@echo "  make prepare       - Проверить структуру и файлы"
	@echo "  make debug         - Отладочная сборка"
	@echo "  make release       - Релизная сборка"
	@echo "  make clean         - Очистить проект"
	@echo ""
	@echo "$(PURPLE)🧪 Тестирование:$(NC)"
	@echo "  make test          - Запустить все тесты"
	@echo "  make test-protocols - Тесты протоколов (TCP/UDP)"
	@echo "  make test-validation - Тесты валидации графов"
	@echo "  make test-input    - Тесты методов ввода"
	@echo "  make quick-test    - Быстрые тесты"
	@echo "  make report        - Показать отчет"
	@echo ""
	@echo "$(CYAN)🚀 Запуск:$(NC)"
	@echo "  make run-server-tcp - Запустить TCP сервер (порт 8080)"
	@echo "  make run-server-udp - Запустить UDP сервер (порт 12345)"
	@echo "  make run-client-tcp - Запустить TCP клиент"
	@echo "  make run-client-udp - Запустить UDP клиент"
	@echo "  make kill-server   - Остановить все серверы"
	@echo ""
	@echo "$(YELLOW)🛠️  Утилиты:$(NC)"
	@echo "  make check-structure - Проверить структуру проекта"
	@echo "  make check          - Быстрая проверка"
	@echo "  make ls             - Показать структуру"
	@echo "  make help          - Показать эту справку"