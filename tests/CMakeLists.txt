cmake_minimum_required(VERSION 3.10)
project(TestFeatures)

# Definir o padrão C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar pkg-config
find_package(PkgConfig REQUIRED)
find_package(SDL2 REQUIRED)

# Encontrar a biblioteca Lua usando pkg-config
pkg_check_modules(LUA REQUIRED lua5.3)


# Incluir diretórios de cabeçalhos
include_directories(${LUA_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ../thirdparty source/)

# Adicionar o executável
add_executable(testFeatures main.cpp)

# Linkar as bibliotecas
target_link_libraries(testFeatures ${LUA_LIBRARIES} ${SDL2_LIBRARIES})