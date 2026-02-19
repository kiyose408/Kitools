cmake_minimum_required(VERSION 3.16)
project(PCProductivityToolbox VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    add_compile_options(/Zc:__cplusplus /utf-8)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

set(SOURCES
    src/main.cpp
    src/core/mainwindow.cpp
    src/core/mainwindow.h
    src/modules/timer/timercontroller.cpp
    src/modules/timer/timercontroller.h
    src/modules/timer/overlaywidget.cpp
    src/modules/timer/overlaywidget.h
    src/modules/timer/timersettingspanel.cpp
    src/modules/timer/timersettingspanel.h
    src/modules/timer/settingsmanager.cpp
    src/modules/timer/settingsmanager.h
    src/modules/todo/todocontroller.cpp
    src/modules/todo/todocontroller.h
    src/modules/todo/desktoptodowidget.cpp
    src/modules/todo/desktoptodowidget.h
    src/modules/todo/taskitemwidget.cpp
    src/modules/todo/taskitemwidget.h
    src/modules/todo/taskmanager.cpp
    src/modules/todo/taskmanager.h
    src/modules/todo/logmanager.cpp
    src/modules/todo/logmanager.h
    src/modules/todo/todosettingspanel.cpp
    src/modules/todo/todosettingspanel.h
    src/modules/todo/tododatatypes.h
)

set(HEADERS
    src/core/mainwindow.h
    src/modules/timer/timercontroller.h
    src/modules/timer/overlaywidget.h
    src/modules/timer/timersettingspanel.h
    src/modules/timer/settingsmanager.h
    src/modules/todo/todocontroller.h
    src/modules/todo/desktoptodowidget.h
    src/modules/todo/taskitemwidget.h
    src/modules/todo/taskmanager.h
    src/modules/todo/logmanager.h
    src/modules/todo/todosettingspanel.h
    src/modules/todo/tododatatypes.h
)

set(RESOURCES
    assets/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/timer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/todo
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
