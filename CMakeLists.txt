cmake_minimum_required(VERSION 3.5)

# group CMake predefined targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# include the project setting file
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME MayaCinematicCameraHUD)

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

# set output directory for the Maya plugin (.mll)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "../release/plug-ins/windows/2026"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "../release/plug-ins/windows/2026"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "../release/plug-ins/windows/2026"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "../release/plug-ins/windows/2026"
)

# output .pdb to build/pdb
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG              "/pdb"
    PDB_OUTPUT_DIRECTORY_RELEASE            "/pdb"
)