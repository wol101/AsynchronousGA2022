cmake_minimum_required(VERSION 3.5)

project(AsynchronousGA2022Qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        AsynchronousGAQtWidget.cpp
        AsynchronousGAQtWidget.h
        AsynchronousGAQtWidget.ui
        BasicXMLSyntaxHighlighter.cpp
        BasicXMLSyntaxHighlighter.h
        DataFile.cpp
        DataFile.h
        GAParametersDialog.cpp
        GAParametersDialog.h
        GAParametersDialog.ui
        MergeExpressionParser.cpp
        MergeExpressionParser.h
        MergeUtil.h
        MergeXML.cpp
        MergeXML.h
        Resources.qrc
        TextEditDialog.cpp
        TextEditDialog.h
        TextEditDialog.ui
        Utilities.cpp
        Utilities.h
        XMLContainer.cpp
        XMLContainer.h
        XMLContainerList.cpp
        XMLContainerList.h
        XMLConverter.cpp
        XMLConverter.h
        main.cpp
        ../pystring/pystring.cpp
        ../pystring/pystring.h
        ../exprtk/exprtk.hpp
        Images/myapp.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AsynchronousGA2022Qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET AsynchronousGA2022Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(AsynchronousGA2022Qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(AsynchronousGA2022Qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(AsynchronousGA2022Qt PRIVATE
    ../exprtk
    ../pystring
    ../rapidxml-1.13
)

target_link_libraries(AsynchronousGA2022Qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(AsynchronousGA2022Qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AsynchronousGA2022Qt)
endif()

if (MSVC)
    target_compile_options(AsynchronousGA2022Qt PRIVATE /bigobj)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

find_package(Qt6 REQUIRED COMPONENTS Xml)
target_link_libraries(AsynchronousGA2022Qt PRIVATE Qt6::Xml)

