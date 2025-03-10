cmake_minimum_required(VERSION 3.5)

project(nurse_BT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/behaviortree.cpp/4.6.2" ${CMAKE_PREFIX_PATH})

find_package(behaviortree_cpp_v3)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

add_executable(${PROJECT_NAME} "nurse.cpp")
target_link_libraries(${PROJECT_NAME} BT::behaviortree_cpp_v3 CURL::libcurl nlohmann_json::nlohmann_json)
