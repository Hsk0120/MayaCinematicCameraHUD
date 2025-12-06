cmake_minimum_required(VERSION 3.5)

# include the project setting file
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME CameraHud)

# set SOURCE_FILES
set(SOURCE_FILES
    src/CameraHudMain.cpp
    src/CameraHudManager.h
    src/CameraHudUtility.h
)

# set linking libraries
set(LIBRARIES
     OpenMaya
     OpenMayaAnim
     OpenMayaUI
     OpenMayaRender
     Foundation
)

# Build plugin
build_plugin()