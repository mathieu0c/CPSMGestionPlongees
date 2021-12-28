# Generated from CPSMGestionPlongees.pro.

cmake_minimum_required(VERSION 3.16)
project(CPSMGestionPlongees LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Sql REQUIRED)

add_executable(CPSMGestionPlongees #WIN32 MACOSX_BUNDLE
    Data/Database.cpp Data/Database.hpp
    Data/global.cpp
    Data/local.cpp
    Debug/DbVisualizer.cpp Debug/DbVisualizer.h Debug/DbVisualizer.ui
    Debug/LogFile.cpp Debug/LogFile.hpp
    Debug/Utils.cpp Debug/Utils.h
    Debug/global.cpp Debug/global.hpp
    GUI/Dialog_ConfirmDiverDeletion.cpp GUI/Dialog_ConfirmDiverDeletion.h GUI/Dialog_ConfirmDiverDeletion.ui
    GUI/Dialog_EditFamily.cpp GUI/Dialog_EditFamily.h GUI/Dialog_EditFamily.ui
    GUI/DiveEdit.cpp GUI/DiveEdit.h GUI/DiveEdit.ui
    GUI/DiveSearch.cpp GUI/DiveSearch.hpp GUI/DiveSearch.ui
    GUI/DiverEdit.cpp GUI/DiverEdit.h GUI/DiverEdit.ui
    GUI/DiverSearch.cpp GUI/DiverSearch.hpp GUI/DiverSearch.ui
    GUI/MainWindow.cpp GUI/MainWindow.h GUI/MainWindow.ui
    GUI/constSettings.hpp
    GUI/global.cpp GUI/global.hpp
    Info/Address.cpp Info/Address.h
    Info/Dive.cpp Info/Dive.h
    Info/Diver.cpp Info/Diver.h
    Info/Generic.hpp
    Info/User.cpp Info/User.h
    Info/global.cpp Info/global.hpp
    global.hpp
    main.cpp
)
target_link_libraries(CPSMGestionPlongees PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets
)

#if((QT_MAJOR_VERSION GREATER 4))
#    target_link_libraries(CPSMGestionPlongees PRIVATE
#        Qt6::Widgets
#    )
#endif()

#install(TARGETS CPSMGestionPlongees
#    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
#    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
#    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
#)
