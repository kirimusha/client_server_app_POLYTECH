# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(ClientServerApp)

# Используем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Флаги компиляции для предупреждений
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Подключаем директорию с заголовочными файлами
include_directories(${CMAKE_SOURCE_DIR}/source)

# ===================== ОБЩИЕ ФАЙЛЫ ======================
# Эти файлы используются и клиентом, и сервером
set(COMMON_SOURCES
    source/common/Graph.cpp
    source/common/Protocol.cpp
)

# ===================== УТИЛИТЫ ======================
# Вспомогательные классы
set(UTILS_SOURCES
    source/utils/InputParser.cpp
    source/utils/Validator.cpp
    source/utils/Logger.cpp
)

# ===================== СЕРВЕРНАЯ ЧАСТЬ ======================
# Исходники сервера
set(SERVER_SOURCES
    source/server/Server.cpp
    source/server/ServerMain.cpp
    ${COMMON_SOURCES}
    ${UTILS_SOURCES}
)

# Создаём исполняемый файл сервера
add_executable(server ${SERVER_SOURCES})

# Подключаем библиотеку для работы с потоками (pthread)
target_link_libraries(server pthread)

# ===================== КЛИЕНТСКАЯ ЧАСТЬ ======================
# Исходники клиента
set(CLIENT_SOURCES
    source/client/Client.cpp
    source/client/ClientMain.cpp
    ${COMMON_SOURCES}
    ${UTILS_SOURCES}
)

# Создаём исполняемый файл клиента
add_executable(client ${CLIENT_SOURCES})

# Подключаем библиотеку для работы с потоками
target_link_libraries(client pthread)

# ===================== УСТАНОВКА ======================
# Указываем, куда устанавливать исполняемые файлы
install(TARGETS server client
    RUNTIME DESTINATION bin
)