cmake_minimum_required(VERSION 3.13.4)
 
project(QtTests)

set(QT5_MODULES
  Core
  Gui
  Widgets)

foreach(module ${QT5_MODULES})
  find_package(Qt5${module} REQUIRED)
endforeach()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PROJECT_NAME QtMinesweeper)

set(SOURCE_FILES
	src/main.cpp
	src/MainWindow.cpp
  src/RightClickableButton.cpp
  src/HighScore.cpp)

set(HEADER_FILES
	include/MainWindow.h
  include/RightClickableButton.h
  include/HighScore.h)

set(UI_FILES
	src/MainWindow.ui)

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${UI_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

source_group(GeneratedFiles FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/moc_compilation.cpp)

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})
source_group(ui FILES ${UI_FILES})

source_group(Resources FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

foreach(module ${QT5_MODULES})
  target_include_directories(${PROJECT_NAME} PUBLIC ${Qt5${module}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} Qt5::${module})
endforeach()

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)