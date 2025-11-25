cmake_minimum_required(VERSION 3.15)

project(MyProjectName LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    main.cpp
    StochasticProcess.cpp
    WienerProcess.cpp
    JumpDiffusionProcess.cpp
    BatesProcess.cpp
    HestonProcess.cpp
    MeanRevertingProcess.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})



target_link_libraries(${PROJECT_NAME} )


