cmake_minimum_required(VERSION 3.10)
project(backend)

set(CMAKE_CXX_STANDARD 17)

add_executable(backend
    src/main.cpp
    src/core/Server.cpp
    src/core/RequestRouter.cpp
    src/core/Locker.cpp
    src/security/Whitelist.cpp
    src/security/Monitor.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)
target_include_directories(backend PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(backend PRIVATE pthread)

#find_package(nlohmann_json REQUIRED) #notrequired if locall installed in include
#target_link_libraries(backend PRIVATE nlohmann_json::nlohmann_json pthread)