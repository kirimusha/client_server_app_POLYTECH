#!/bin/bash
# Генерация отчетов о тестировании

generate_report() {
    print_header "ИТОГОВЫЙ ОТЧЕТ"

    echo "Дата тестирования: $(date)"
    echo "Всего тестов:       $TOTAL_TESTS"
    echo "Успешно:            $PASSED_TESTS"
    echo "Неудачно:           $FAILED_TESTS"
    echo ""

    # Расчет процента успеха
    if [ $TOTAL_TESTS -gt 0 ]; then
        local success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        echo "Процент успеха:     $success_rate%"
    fi

    # Сохранение отчета
    cat > "$SUMMARY_FILE" << EOF
ОТЧЕТ О ТЕСТИРОВАНИИ
====================
Дата: $(date)
Версия тестов: 1.0

СТАТИСТИКА:
- Всего тестов: $TOTAL_TESTS
- Успешно:      $PASSED_TESTS
- Неудачно:     $FAILED_TESTS
- Процент успеха: $success_rate%

ДЕТАЛИ:
$(cat "$RESULTS_FILE" 2>/dev/null || echo "Детали недоступны")

КОНЕЦ ОТЧЕТА
EOF

    echo ""
    echo "Детальный лог: $RESULTS_FILE"
    echo "Краткий отчет: $SUMMARY_FILE"

    # Вывод результата
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "\n${GREEN}==========================================${NC}"
        echo -e "${GREEN}  ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!${NC}"
        echo -e "${GREEN}==========================================${NC}"
        return 0
    else
        echo -e "\n${RED}==========================================${NC}"
        echo -e "${RED}  ЕСТЬ НЕУДАЧНЫЕ ТЕСТЫ: $FAILED_TESTS${NC}"
        echo -e "${RED}==========================================${NC}"
        return 1
    fi
}