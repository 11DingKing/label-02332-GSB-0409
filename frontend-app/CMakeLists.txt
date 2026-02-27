cmake_minimum_required(VERSION 3.16)
project(GameConsole VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql SvgWidgets)

set(SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/gamebase.h
    src/gamebase.cpp
    src/gameselectwidget.h
    src/gameselectwidget.cpp
    src/difficultyselectwidget.h
    src/difficultyselectwidget.cpp
    src/historymanager.h
    src/historymanager.cpp
    src/historywidget.h
    src/historywidget.cpp
    src/minesweepergame.h
    src/minesweepergame.cpp
    src/snakegame.h
    src/snakegame.cpp
    src/gomokugame.h
    src/gomokugame.cpp
    src/reversigame.h
    src/reversigame.cpp
    src/tetrisgame.h
    src/tetrisgame.cpp
)

set(RESOURCES
    resources/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::SvgWidgets
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "GameConsole"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.gameconsole.app"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
endif()
