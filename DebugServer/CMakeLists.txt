cmake_minimum_required(VERSION 3.5)

project(HuaweiRouter LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(include)
if(ANDROID)
  add_library(HuaweiRouter SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    oglwidget.cpp
    oglwidget.h
    graph.cpp
    graph.h
    channel.h
    channel.cpp
    node.h
    node.cpp
    network.h
    network.cpp
    dir/channel.cpp
          SimulationCore/ServerNode.cpp SimulationCore/ServerNode.h SimulationCore/ServerConnection.cpp SimulationCore/ServerConnection.h SimulationCore/DebugServer.cpp SimulationCore/DebugServer.h TextureImage.cpp TextureImage.h Utils/MutexInt.cpp Utils/MutexInt.h Utils/AsyncVar.cpp Utils/AsyncVar.h)
else()
  add_executable(HuaweiRouter
          Network/main.cpp
          mainwindow.cpp
          mainwindow.h
          mainwindow.ui
          oglwidget.cpp
          oglwidget.h
          graph.cpp
          graph.h
          Network/channel.h
          Network/channel.cpp
          Network/node.h
          Network/node.cpp
          Network/network.h
          Network/network.cpp
          Network/channel.cpp
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
          SimulationCore/DebugServer.cpp SimulationCore/DebugServer.h Utils/Settings.h Utils/ColorMode.cpp Utils/ColorMode.h TextureImage.cpp TextureImage.h Utils/MutexInt.cpp Utils/MutexInt.h Utils/AsyncVar.cpp Utils/AsyncVar.h)
endif()

target_link_libraries(HuaweiRouter PRIVATE Qt5::Widgets  ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
