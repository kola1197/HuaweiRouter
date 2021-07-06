cmake_minimum_required(VERSION 3.5)

project(HuaweiRouter LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -pthread")
option(USE_OPENCV "Should compile with OpenCV" YES)
IF ( USE_OPENCV )
    MESSAGE( STATUS "Including OpenCV on DroneOnboardController build" )
    find_package( OpenCV 3.4 REQUIRED)
    if (OpenCV_LIBS)
        add_definitions(-DWITH_OPENCV)
    else()
        message("You requested OPENCV in the build, but none was found.")
    endif()
ENDIF ()
find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)



include_directories(include)
add_executable(HuaweiRouter
          Network/main.cpp
          mainwindow.cpp
          mainwindow.h
          mainwindow.ui
          oglwidget.cpp
          oglwidget.h
          graph.cpp
          graph.h
          SimulationCore/simulation.cpp
          SimulationCore/simulation.h
          SimulationCore/Messages.h
          SimulationCore/Messages.cpp
          SimulationCore/ServerNode.cpp
          SimulationCore/ServerNode.h
          SimulationCore/ServerConnection.cpp
          SimulationCore/ServerConnection.h
          Utils/MutexBool.cpp
          Utils/MutexBool.h
          SimulationCore/DebugServer.cpp
          SimulationCore/DebugServer.h
          Utils/Settings.h Utils/ColorMode.cpp
          Utils/ColorMode.h
          Utils/MutexInt.cpp
          Utils/MutexInt.h
          Utils/AsyncVar.cpp
          Utils/AsyncVar.h
          settingsform.cpp
          settingsform.h
          settingsform.ui
          Utils/Settings.cpp
          Utils/sout.cpp
          Utils/sout.h
          SimulationCore/SendingQueue.cpp
          SimulationCore/SendingQueue.h
          Utils/CpuInfo.cpp
          Utils/CpuInfo.h
          SimulationCore/SimulationReport.cpp
          SimulationCore/SimulationReport.h
          icon.qrc SimulationCore/LocalVoting.cpp SimulationCore/LocalVoting.h)

target_link_libraries(HuaweiRouter PRIVATE Qt5::Widgets  ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${OpenCV_LIBS})
