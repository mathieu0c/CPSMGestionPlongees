# Generated from CPSMGestionPlongees.pro.

#set(PROJECT_NAME)

#VERSION = MAJOR.MINOR.PATCH
set(VERSIONING_MAJOR 0)#incompatible update
set(VERSIONING_MINOR 0)#new features added
set(VERSIONING_PATCH 0)#bug fixed / minor change


add_definitions("-DPROJECT_V_MAJOR=${VERSIONING_MAJOR}")
add_definitions("-DPROJECT_V_MINOR=${VERSIONING_MINOR}")
add_definitions("-DPROJECT_V_PATCH=${VERSIONING_PATCH}")

add_compile_definitions(PROJECT_GITHUB_RELEASE="https://api.github.com/repos/mathieu0c/CPSMGestionPlongees/releases/latest")

cmake_minimum_required(VERSION 3.16)
project(CPSMGestionPlongees VERSION ${VERSIONING_MAJOR}.${VERSIONING_MINOR}.${VERSIONING_PATCH} LANGUAGES CXX)
add_definitions("-DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")

set(EXE_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(COMPILED_FOR_RELEASE)
endif()



find_package(Qt6 COMPONENTS
                 Core
                 Gui
                 Widgets
                 Sql
                 Network
                 REQUIRED)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/iconResource.rc")

set(PROJECT_SOURCES
    Update/Update.hpp Update/Update.cpp
    Update/PostUpdate.hpp Update/PostUpdate.cpp
    Update/UpdateManager.hpp Update/UpdateManager.cpp Update/UpdateManager.ui

    DBApi/Database.cpp DBApi/Database.hpp
    DBApi/global.cpp
    DBApi/local.cpp
    DBApi/Generic.hpp
    DBApi/DataStructs.hpp DBApi/DataStructs.cpp
    DBApi/DBAddress.hpp DBApi/DBAddress.cpp
    DBApi/DBDive.hpp DBApi/DBDive.cpp
    DBApi/DBDiver.hpp DBApi/DBDiver.cpp
    DBApi/DBDiverLevel.hpp DBApi/DBDiverLevel.cpp
    DBApi/DBApi.hpp DBApi/DBApi.cpp

    Debug/ScopeTimer.hpp Debug/ScopeTimer.cpp
    Debug/DbVisualizer.cpp Debug/DbVisualizer.h Debug/DbVisualizer.ui
    Debug/LogFile.cpp Debug/LogFile.hpp
    Debug/Utils.cpp Debug/Utils.h
    Debug/global.cpp Debug/global.hpp

    GUI/DiveMembersEditor.hpp GUI/DiveMembersEditor.cpp GUI/DiveMembersEditor.ui
    GUI/Dialog_ConfirmDiveDeletion.hpp GUI/Dialog_ConfirmDiveDeletion.cpp GUI/Dialog_ConfirmDiveDeletion.ui
    GUI/Dialog_ConfirmDiverDeletion.cpp GUI/Dialog_ConfirmDiverDeletion.h GUI/Dialog_ConfirmDiverDeletion.ui
    GUI/Dialog_EditFamily.cpp GUI/Dialog_EditFamily.h GUI/Dialog_EditFamily.ui
    GUI/DiveEdit.cpp GUI/DiveEdit.h GUI/DiveEdit.ui
    GUI/DiveSearch.cpp GUI/DiveSearch.hpp GUI/DiveSearch.ui
    GUI/DiverEdit.cpp GUI/DiverEdit.h GUI/DiverEdit.ui
    GUI/DiverSearch.cpp GUI/DiverSearch.hpp GUI/DiverSearch.ui
    GUI/MainWindow.cpp GUI/MainWindow.h GUI/MainWindow.ui
    GUI/constSettings.hpp
    GUI/global.cpp GUI/global.hpp


    Utils/JSONTools.hpp Utils/JSONTools.cpp
    Utils/GUITools.hpp Utils/GUITools.cpp

    global.hpp
    main.cpp
)

qt_add_executable(${EXE_NAME} WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_link_libraries(${EXE_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

#----- Used to get the current filename

# The additional / is important to remove the last character from the path.
# Note that it does not matter if the OS uses / or \, because we are only
# saving the path size.
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")



message("Project name : <${PROJECT_NAME}>")
