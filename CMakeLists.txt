cmake_minimum_required(VERSION 3.10)
project(client_server_app VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Политики CMake
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

# Опции сборки
option(BUILD_TESTS "Build automated tests" ON)
option(BUILD_TEST_REPORTS "Generate test reports" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Указываем пути к заголовочным файлам
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/client
        ${CMAKE_CURRENT_SOURCE_DIR}/server
)

# Настройка выходных каталогов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Флаги компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)

    if(ENABLE_COVERAGE)
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
    endif()
endif()

# Сервер
add_executable(server
        server/ServerMain.cpp
        server/Server.cpp
        common/Graph.cpp
        common/Protocol.cpp
        utils/FileReader.cpp
        utils/InputParser.cpp
        utils/Validator.cpp
        utils/Logger.cpp
)

# Клиент
add_executable(client
        client/ClientMain.cpp
        client/Client.cpp
        common/Graph.cpp
        common/Protocol.cpp
        utils/FileReader.cpp
        utils/InputParser.cpp
        utils/Validator.cpp
        utils/Logger.cpp
)

# Для многопоточности
find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
target_link_libraries(client Threads::Threads)

# ================================================
# ТЕСТИРОВАНИЕ
# ================================================

if(BUILD_TESTS)
    message(STATUS "Building tests...")

    # Поиск необходимых инструментов
    find_program(EXPECT_EXECUTABLE expect)
    find_program(BASH_EXECUTABLE bash)
    find_program(PYTHON_EXECUTABLE python3)

    if(NOT EXPECT_EXECUTABLE)
        message(WARNING "expect not found. Install with: sudo apt-get install expect")
        set(BUILD_TESTS OFF)
    endif()

    if(NOT BASH_EXECUTABLE)
        message(WARNING "bash not found")
        set(BUILD_TESTS OFF)
    endif()
endif()

if(BUILD_TESTS)
    # Создаем каталог для тестов в build
    set(TEST_BUILD_DIR ${CMAKE_BINARY_DIR}/tests)
    file(MAKE_DIRECTORY ${TEST_BUILD_DIR})

    # Список тестовых скриптов для копирования
    set(TEST_SCRIPTS
            run_tests.sh
            config.sh
            utils.sh
            setup.sh
            cleanup.sh
            report.sh
    )

    # Копируем основные скрипты
    foreach(script ${TEST_SCRIPTS})
        configure_file(
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/${script}
                ${TEST_BUILD_DIR}/${script}
                COPYONLY
        )
    endforeach()

    # Делаем скрипты исполняемыми
    add_custom_command(
            TARGET client POST_BUILD
            COMMAND chmod +x ${TEST_BUILD_DIR}/*.sh
            COMMENT "Making test scripts executable"
    )

    # Создаем подкаталоги для тестов
    set(TEST_SUBDIRS protocols input_methods validation algorithms)
    foreach(subdir ${TEST_SUBDIRS})
        file(MAKE_DIRECTORY ${TEST_BUILD_DIR}/${subdir})

        # Копируем тестовые скрипты в подкаталоги
        file(GLOB test_files ${CMAKE_CURRENT_SOURCE_DIR}/tests/${subdir}/*.expect)
        foreach(test_file ${test_files})
            get_filename_component(test_name ${test_file} NAME)
            configure_file(
                    ${test_file}
                    ${TEST_BUILD_DIR}/${subdir}/${test_name}
                    COPYONLY
            )
            # Делаем expect файлы исполняемыми
            add_custom_command(
                    TARGET client POST_BUILD
                    COMMAND chmod +x ${TEST_BUILD_DIR}/${subdir}/${test_name}
                    COMMENT "Making ${test_name} executable"
            )
        endforeach()
    endforeach()

    # Обновляем config.sh для CMake сборки
    configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/config.sh
            ${TEST_BUILD_DIR}/config.sh
            @ONLY
    )

    # Добавляем цель для запуска тестов
    add_custom_target(run-tests
            COMMAND ${BASH_EXECUTABLE} ${TEST_BUILD_DIR}/run_tests.sh
            DEPENDS server client
            WORKING_DIRECTORY ${TEST_BUILD_DIR}
            COMMENT "Running automated tests"
    )

    # Цели для запуска отдельных групп тестов
    add_custom_target(test-protocols
            COMMAND ${BASH_EXECUTABLE} -c "cd ${TEST_BUILD_DIR} && for test in protocols/*.expect; do echo \"Running \$$test...\"; expect \$$test && echo '✓ PASSED' || echo '✗ FAILED'; done"
            DEPENDS server client
            WORKING_DIRECTORY ${TEST_BUILD_DIR}
            COMMENT "Running protocol tests"
    )

    add_custom_target(test-validation
            COMMAND ${BASH_EXECUTABLE} -c "cd ${TEST_BUILD_DIR} && for test in validation/*.expect; do echo \"Running \$$test...\"; expect \$$test && echo '✓ PASSED' || echo '✗ FAILED'; done"
            DEPENDS server client
            WORKING_DIRECTORY ${TEST_BUILD_DIR}
            COMMENT "Running validation tests"
    )

    add_custom_target(test-input
            COMMAND ${BASH_EXECUTABLE} -c "cd ${TEST_BUILD_DIR} && for test in input_methods/*.expect; do echo \"Running \$$test...\"; expect \$$test && echo '✓ PASSED' || echo '✗ FAILED'; done"
            DEPENDS server client
            WORKING_DIRECTORY ${TEST_BUILD_DIR}
            COMMENT "Running input tests"
    )

    # Быстрые тесты
    add_custom_target(quick-test
            COMMAND ${BASH_EXECUTABLE} -c "
            cd ${TEST_BUILD_DIR} &&
            echo 'Running quick tests...' &&
            expect protocols/test_tcp_basic.expect &&
            expect protocols/test_udp_basic.expect &&
            expect validation/test_graph_at_min.expect &&
            expect validation/test_graph_at_max.expect &&
            echo '✓ All quick tests passed' ||
            echo '✗ Some quick tests failed'"
            DEPENDS server client
            WORKING_DIRECTORY ${TEST_BUILD_DIR}
            COMMENT "Running quick tests"
    )

    # Цель для показа отчета
    add_custom_target(test-report
            COMMAND ${BASH_EXECUTABLE} -c "ls -t ${TEST_BUILD_DIR}/summary_*.txt 2>/dev/null | head -1 | xargs cat 2>/dev/null || echo 'No test reports found'"
            COMMENT "Showing latest test report"
    )

    # Цель для очистки тестов
    add_custom_target(clean-tests
            COMMAND ${BASH_EXECUTABLE} -c "cd ${TEST_BUILD_DIR} && rm -rf test_files results_*.log summary_*.txt"
            COMMENT "Cleaning test files"
    )

    # Добавляем зависимость тестов от сборки
    add_dependencies(run-tests server client)

    message(STATUS "Tests will be built in: ${TEST_BUILD_DIR}")
endif()

# ================================================
# UNIT ТЕСТЫ (опционально)
# ================================================

option(BUILD_UNIT_TESTS "Build unit tests with GoogleTest" OFF)

if(BUILD_UNIT_TESTS)
    # Используем FetchContent для GoogleTest
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0
    )

    FetchContent_MakeAvailable(googletest)

    # Unit тесты для Graph
    add_executable(graph_test
            tests/unit/GraphTest.cpp
            common/Graph.cpp
            utils/Validator.cpp
    )
    target_link_libraries(graph_test gtest_main)
    target_include_directories(graph_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common)

    # Unit тесты для Protocol
    add_executable(protocol_test
            tests/unit/ProtocolTest.cpp
            common/Protocol.cpp
    )
    target_link_libraries(protocol_test gtest_main)
    target_include_directories(protocol_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common)

    # Добавляем тесты в CTest
    include(CTest)
    add_test(NAME GraphTest COMMAND graph_test)
    add_test(NAME ProtocolTest COMMAND protocol_test)

    # Цель для запуска unit-тестов
    add_custom_target(run-unit-tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            DEPENDS graph_test protocol_test
            COMMENT "Running unit tests"
    )
endif()

# ================================================
# КОДОВОЕ ПОКРЫТИЕ
# ================================================

if(ENABLE_COVERAGE)
    find_program(GCOVR_EXECUTABLE gcovr)

    if(GCOVR_EXECUTABLE)
        add_custom_target(coverage
                COMMAND ${GCOVR_EXECUTABLE} --exclude tests/ --exclude external/ --html --html-details -o ${CMAKE_BINARY_DIR}/coverage.html
                COMMAND ${GCOVR_EXECUTABLE} --exclude tests/ --exclude external/ -o ${CMAKE_BINARY_DIR}/coverage.txt
                DEPENDS run-tests
                COMMENT "Generating code coverage report"
        )
    endif()
endif()

# ================================================
# ДОКУМЕНТАЦИЯ (опционально)
# ================================================

option(BUILD_DOCS "Build documentation with Doxygen" OFF)

if(BUILD_DOCS)
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
        )
    endif()
endif()

# ================================================
# УСТАНОВКА
# ================================================

# Установка исполняемых файлов
install(TARGETS server client
        RUNTIME DESTINATION bin
        COMPONENT Runtime
)

# Установка документации (если есть)
if(BUILD_DOCS AND DOXYGEN_FOUND)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/html/
            DESTINATION share/doc/${PROJECT_NAME}
            COMPONENT Documentation
    )
endif()

# ================================================
# ИНФОРМАЦИЯ О ПРОЕКТЕ
# ================================================

message(STATUS "")
message(STATUS "==========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Build targets:")
message(STATUS "  server: ${CMAKE_BINARY_DIR}/bin/server")
message(STATUS "  client: ${CMAKE_BINARY_DIR}/bin/client")
message(STATUS "")
if(BUILD_TESTS)
    message(STATUS "Tests: ENABLED")
    message(STATUS "  Test directory: ${TEST_BUILD_DIR}")
    message(STATUS "  Available test targets:")
    message(STATUS "    make run-tests      - Run all tests")
    message(STATUS "    make test-protocols - Protocol tests")
    message(STATUS "    make test-validation - Validation tests")
    message(STATUS "    make test-input     - Input method tests")
    message(STATUS "    make quick-test     - Quick tests")
    message(STATUS "    make test-report    - Show test report")
else()
    message(STATUS "Tests: DISABLED")
endif()
if(BUILD_UNIT_TESTS)
    message(STATUS "Unit tests: ENABLED")
else()
    message(STATUS "Unit tests: DISABLED")
endif()
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage: ENABLED")
endif()
message(STATUS "==========================================")