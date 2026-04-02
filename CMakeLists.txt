cmake_minimum_required(VERSION 3.16)
project(PCProductivityToolbox VERSION 1.2.0 LANGUAGES CXX)

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
    src/core/globalshortcutmanager.cpp
    src/core/globalshortcutmanager.h
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
    src/modules/notes/notescontroller.cpp
    src/modules/notes/notescontroller.h
    src/modules/notes/noteswidget.cpp
    src/modules/notes/noteswidget.h
    src/modules/notes/notesitemwidget.cpp
    src/modules/notes/notesitemwidget.h
    src/modules/notes/notesmanager.cpp
    src/modules/notes/notesmanager.h
    src/modules/notes/notesreminder.cpp
    src/modules/notes/notesreminder.h
    src/modules/notes/notessettingspanel.cpp
    src/modules/notes/notessettingspanel.h
    src/modules/notes/notesdatatypes.h
    src/modules/clipboard/clipboardcontroller.cpp
    src/modules/clipboard/clipboardcontroller.h
    src/modules/clipboard/clipboardwidget.cpp
    src/modules/clipboard/clipboardwidget.h
    src/modules/clipboard/clipboarditemwidget.cpp
    src/modules/clipboard/clipboarditemwidget.h
    src/modules/clipboard/clipboardmanager.cpp
    src/modules/clipboard/clipboardmanager.h
    src/modules/clipboard/clipboardsettingspanel.cpp
    src/modules/clipboard/clipboardsettingspanel.h
    src/modules/clipboard/clipboarddatatypes.h
    src/modules/clipboard/clipboarddatatypes.cpp
    src/modules/launcher/launcherdatatypes.h
    src/modules/launcher/launcherindexer.cpp
    src/modules/launcher/launcherindexer.h
    src/modules/launcher/launchersettingspanel.cpp
    src/modules/launcher/launchersettingspanel.h
    src/modules/launcher/launchercontroller.cpp
    src/modules/launcher/launchercontroller.h
    src/modules/timetracker/timetrackerdatatypes.h
    src/modules/timetracker/windowmonitor.cpp
    src/modules/timetracker/windowmonitor.h
    src/modules/timetracker/activitymanager.cpp
    src/modules/timetracker/activitymanager.h
    src/modules/timetracker/timetrackercontroller.cpp
    src/modules/timetracker/timetrackercontroller.h
    src/modules/timetracker/timetrackersettingspanel.cpp
    src/modules/timetracker/timetrackersettingspanel.h
    src/core/autostartmanager.cpp
    src/core/autostartmanager.h
)

set(HEADERS
    src/core/mainwindow.h
    src/core/globalshortcutmanager.h
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
    src/modules/notes/notescontroller.h
    src/modules/notes/noteswidget.h
    src/modules/notes/notesitemwidget.h
    src/modules/notes/notesmanager.h
    src/modules/notes/notesreminder.h
    src/modules/notes/notessettingspanel.h
    src/modules/notes/notesdatatypes.h
    src/modules/clipboard/clipboardcontroller.h
    src/modules/clipboard/clipboardwidget.h
    src/modules/clipboard/clipboarditemwidget.h
    src/modules/clipboard/clipboardmanager.h
    src/modules/clipboard/clipboardsettingspanel.h
    src/modules/clipboard/clipboarddatatypes.h
    src/modules/launcher/launchercontroller.h
    src/modules/launcher/launcherindexer.h
    src/modules/launcher/launchersettingspanel.h
    src/modules/launcher/launcherdatatypes.h
    src/modules/timetracker/timetrackerdatatypes.h
    src/modules/timetracker/windowmonitor.h
    src/modules/timetracker/activitymanager.h
    src/modules/timetracker/timetrackercontroller.h
    src/modules/timetracker/timetrackersettingspanel.h
    src/core/autostartmanager.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/notes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/clipboard
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/launcher
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules/timetracker
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE psapi)
endif()
