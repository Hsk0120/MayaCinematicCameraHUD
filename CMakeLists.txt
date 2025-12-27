cmake_minimum_required(VERSION 3.5)

# group CMake predefined targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# include the project setting file
include("$ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake")

# Maya 2026以降: find_qtマクロを使用
if(MAYA_VERSION VERSION_GREATER_EQUAL 2026)
    find_qt(Core Widgets Gui)
endif()

# Maya 2022-2024: 手動でQt5を設定
if(MAYA_VERSION VERSION_LESS 2026)
    # MayaにはRelease版のQtしかないため、Debug構成でもRelease版を使用
    # この設定はfind_packageより前に行う必要がある
    set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Release;RelWithDebInfo;MinSizeRel;")
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Release;RelWithDebInfo;MinSizeRel;")
    
    # Qt5のファイル存在チェックを無効化（Mayaにはリリース版のみ存在）
    macro(_qt5_Core_check_file_exists file)
        # 何もしない（存在チェックをスキップ）
    endmacro()
    macro(_qt5_Gui_check_file_exists file)
    endmacro()
    macro(_qt5_Widgets_check_file_exists file)
    endmacro()
    
    # Qtプラグインの自動ロードを無効化（存在しないプラグインでエラーになるため）
    set(Qt5_NO_LINK_QTMAIN ON)
    set(CMAKE_AUTOMOC OFF)
    set(CMAKE_AUTORCC OFF)
    set(CMAKE_AUTOUIC OFF)
    
    # Qt5のCMakeパスを設定
    set(CMAKE_PREFIX_PATH "$ENV{DEVKIT_LOCATION}/lib/cmake/Qt5")
    
    # Qt5パッケージを検索
    find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
    
    # Qt関連のライブラリを追加
    set(PACKAGE_LIBS ${PACKAGE_LIBS} Qt5::Core Qt5::Widgets Qt5::Gui)
    
    # Qtインクルードディレクトリを追加
    set(INCLUDE_DIRS ${INCLUDE_DIRS} "$ENV{DEVKIT_LOCATION}/include")
endif()

# specify project name
set(PROJECT_NAME MayaCinematicCameraHUD)

# set SOURCE_FILES
set(SOURCE_FILES
    src/CameraHudMain.cpp
    src/CameraHudUtility.cpp
    src/CameraHudManager.cpp
    src/CameraHudNode.cpp
    src/CameraHudDrawOverride.cpp
    src/CameraHudManager.h
    src/CameraHudUtility.h
    src/CameraHudNode.h
    src/CameraHudDrawOverride.h
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
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "../release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "../release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "../release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "../release/plug-ins/windows/${MAYA_VERSION}"
)

# output .pdb to build/pdb
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG              "/pdb"
    PDB_OUTPUT_DIRECTORY_RELEASE            "/pdb"
)