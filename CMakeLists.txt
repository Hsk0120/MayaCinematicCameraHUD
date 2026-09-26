cmake_minimum_required(VERSION 3.15)

project(MayaCinematicCameraHUD)

# ソースはUTF-8。Mayaのバージョンによって既定の文字コードが違うため明示する
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Visual Studioのソリューション上で、CMakeが自動生成するターゲットをまとめて表示する
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

# devkitの共通設定(Maya APIのインクルードパス・ライブラリ、build_plugin関数など)を読み込む
include("$ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake")

# Maya 2025以降(Qt6): find_qtマクロを使用
if(MAYA_VERSION VERSION_GREATER_EQUAL 2025)
    find_qt(Core Widgets Gui)
endif()

# Maya 2022-2024(Qt5): 手動でQt5を設定
if(MAYA_VERSION VERSION_LESS 2025)
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

# build_pluginが参照する変数。PROJECT_NAMEは出力される.mllのファイル名になる
set(PROJECT_NAME MayaCinematicCameraHUD)

# プラグインを構成するソースとヘッダー
set(SOURCE_FILES
    src/CameraHudMain.cpp
    src/CameraHudUtility.cpp
    src/CameraHudDrawData.cpp
    src/CameraHudNode.cpp
    src/CameraHudDrawOverride.cpp
    src/CameraHudDrawData.h
    src/CameraHudUtility.h
    src/CameraHudNode.h
    src/CameraHudDrawOverride.h
)

# リンクするMaya APIライブラリ
set(LIBRARIES
     OpenMaya
     OpenMayaAnim
     OpenMayaUI
     OpenMayaRender
     Foundation
)

# 上の変数からプラグインのターゲットを作成する
build_plugin()

# 生成した.mllは、Mayaのバージョンごとにrelease/plug-ins/windows/<年>/へ出力する
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/release/plug-ins/windows/${MAYA_VERSION}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${CMAKE_CURRENT_SOURCE_DIR}/release/plug-ins/windows/${MAYA_VERSION}"
)

# デバッグ情報(.pdb)はリリースフォルダに置かず、ビルドフォルダ内のpdb/へ出力する
set_target_properties(${PROJECT_NAME} PROPERTIES
    PDB_OUTPUT_DIRECTORY_DEBUG              "${CMAKE_CURRENT_BINARY_DIR}/pdb"
    PDB_OUTPUT_DIRECTORY_RELEASE            "${CMAKE_CURRENT_BINARY_DIR}/pdb"
)