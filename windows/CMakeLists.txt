cmake_minimum_required(VERSION 3.10)
target_compile_features(your_target PUBLIC cxx_std_17)

# Вказуємо назву проекту
project(MyNapiAddon)

# Встановлюємо шлях до Node.js
find_package(NodeJs REQUIRED)

# Вказуємо шлях до заголовочних файлів N-API
include_directories(${NODEJS_INCLUDE_DIRS})

# Вказуємо шлях до бібліотеки Node.js
link_directories(${NODEJS_LIBRARY_DIRS})

# Описуємо вихідні файли
set(SOURCE_FILES addon.cc)

# Створюємо бібліотеку
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Вказуємо залежність від бібліотеки Node.js
target_link_libraries(${PROJECT_NAME} ${NODEJS_LIBRARIES})
