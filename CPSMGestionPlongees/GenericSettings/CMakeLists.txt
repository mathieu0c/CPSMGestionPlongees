#static lib cmake

set(LIB_BASENAME GenericSettings)
set(LIB_PROJECT ${LIB_BASENAME})

project(${LIB_PROJECT} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(COMPILED_FOR_RELEASE)
endif()

#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

set(LIB_SRC
    src/BaseSetting.hpp src/BaseSetting.cpp
    src/SettingsManager.hpp src/SettingsManager.cpp
    src/JSONTools.hpp src/JSONTools.cpp
)

add_library(${LIB_PROJECT}
    ${LIB_SRC}
)

add_library(${LIB_PROJECT}::${LIB_PROJECT} ALIAS ${LIB_PROJECT})


target_link_libraries(${LIB_PROJECT}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)


#to use from to main CMAKE :
#add_subdirectory(directory)
# target_link_libraries(.....
#     ${LIB_BASENAME}::${LIB_BASENAME})
