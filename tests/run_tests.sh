#!/bin/bash
# Главный скрипт для запуска всех тестов

set -e  # Выход при ошибке

# Импорт конфигурации
source "$(dirname "$0")/config.sh"
source "$(dirname "$0")/utils.sh"
source "$(dirname "$0")/setup.sh"

# Очистка предыдущих запусков
source "$(dirname "$0")/cleanup.sh"

print_header "АВТОМАТИЧЕСКОЕ ТЕСТИРОВАНИЕ"
echo "Дата запуска: $(date)"
echo "Версия тестов: 1.0"
echo ""

# Проверка зависимостей
check_dependencies

# Проверка исполняемых файлов
check_binaries

# Создание тестовой среды
setup_test_environment

# Запуск тестов
echo ""
echo "Запуск тестовых сценариев..."
echo "=============================="

# Протоколы
echo ""
echo "1. ТЕСТИРОВАНИЕ ПРОТОКОЛОВ:"
run_test "protocols/test_tcp_basic.expect" "TCP: Базовая работа"
run_test "protocols/test_tcp_multiple.expect" "TCP: Несколько клиентов"
run_test "protocols/test_udp_basic.expect" "UDP: Базовая работа"
run_test "protocols/test_udp_unavailable.expect" "UDP: Недоступный сервер"
run_test "protocols/test_udp_retransmission.expect" "UDP: Повторная отправка"

# Методы ввода
echo ""
echo "2. ТЕСТИРОВАНИЕ МЕТОДОВ ВВОДА:"
run_test "input_methods/test_keyboard_input.expect" "Ввод с клавиатуры"
run_test "input_methods/test_file_input.expect" "Ввод из файла"

# Валидация ОДЗ
echo ""
echo "3. ТЕСТИРОВАНИЕ ВАЛИДАЦИИ (ОДЗ):"
run_test "validation/test_graph_below_min.expect" "ОДЗ: 5 вершин (ниже минимума)"
run_test "validation/test_graph_at_min.expect" "ОДЗ: 6 вершин (минимум)"
run_test "validation/test_graph_middle.expect" "ОДЗ: 10 вершин (середина)"
run_test "validation/test_graph_at_max.expect" "ОДЗ: 20 вершин (максимум)"
run_test "validation/test_graph_above_max.expect" "ОДЗ: 21 вершина (выше максимума)"

# Алгоритмы
echo ""
echo "4. ТЕСТИРОВАНИЕ АЛГОРИТМОВ:"
run_test "algorithms/test_no_path.expect" "Алгоритм: Несуществующий путь"

# Очистка
echo ""
echo "Очистка тестовой среды..."
cleanup_test_environment

# Генерация отчета
echo ""
source "$(dirname "$0")/report.sh"
generate_report

exit $?