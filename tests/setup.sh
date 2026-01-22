#!/bin/bash
# Настройка тестовой среды

setup_test_environment() {
    echo "Настройка тестовой среды..."

    # Создание каталогов
    mkdir -p "$TEST_FILES_DIR"

    # Создание тестовых файлов
    create_test_files

    echo -e "${GREEN}Тестовая среда настроена${NC}"
}

create_test_files() {
    echo "Создание тестовых файлов..."

    # Граф с 5 вершинами
    cat > "$TEST_GRAPH_5V" << EOF
A B
B C
C D
D E
E A
EOF

    # Граф с 6 вершинами
    cat > "$TEST_GRAPH_6V" << EOF
A B, B C, C D, D E, E F, F A
EOF

    # Граф с 13 вершинами
    cat > "$TEST_GRAPH_13V" << EOF
A B, B C, C D, D E, E F, F G
G H, H I, I J, J K, K L, L M, M A
EOF

    # Граф с 20 вершинами
    cat > "$TEST_GRAPH_20V" << EOF
A B, B C, C D, D E, E F, F G, G H, H I, I J, J K
K L, L M, M N, N O, O P, P Q, Q R, R S, S T, T A
EOF

    # Граф с 21 вершиной
    cat > "$TEST_GRAPH_21V" << EOF
A B, B C, C D, D E, E F, F G, G H, H I, I J, J K
K L, L M, M N, N O, O P, P Q, Q R, R S, S T, T U, U A
EOF

    echo "  Создано 5 тестовых файлов в $TEST_FILES_DIR"
}