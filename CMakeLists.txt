# Versao minima do CMake exigida
cmake_minimum_required(VERSION 3.15)

# Nome do projeto e linguagem usada
project(CuboMagico2x2 CXX)

# Qual padrao de C++ usar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Onde o compilador deve procurar os .hpp (nossos headers)
include_directories(include)

# Lista dos .cpp que formam o executavel final
add_executable(cubo
    src/main.cpp
    src/Cubo.cpp
    src/Busca.cpp
)