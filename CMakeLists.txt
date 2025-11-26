cmake_minimum_required(VERSION 3.15)

project(MyProjectName LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(/opt/homebrew/include)

set(SOURCE_FILES
    main.cpp
    StochasticProcess.cpp
    WienerProcess.cpp
    JumpDiffusionProcess.cpp
    BatesProcess.cpp
    HestonProcess.cpp
    MeanRevertingProcess.cpp
    SimulationApp.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)