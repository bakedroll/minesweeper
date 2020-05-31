cmake_minimum_required(VERSION 3.13.4)
 
project(QtTests)

set(QT5_MODULES
  Core
  Gui
  Widgets)

foreach(module ${QT5_MODULES})
  find_package(Qt5${module} REQUIRED)
endforeach()

find_package(Qt5LinguistTools REQUIRED)

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

set(TS_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_en.ts
  ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_de.ts)

qt5_add_translation(QM_FILES ${TS_FILES})

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${UI_FILES}
  ${QM_FILES}
  ${TS_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

source_group(GeneratedFiles FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/moc_compilation.cpp)

source_group(src FILES ${SOURCE_FILES})
source_group(include FILES ${HEADER_FILES})
source_group(ui FILES ${UI_FILES})
source_group(translations ${QM_FILES} ${TS_FILES})

source_group(Resources FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

# set include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

foreach(module ${QT5_MODULES})
  target_include_directories(${PROJECT_NAME} PUBLIC ${Qt5${module}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} Qt5::${module})
endforeach()

add_custom_target(
  lupdate
  COMMAND lupdate -I ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src -ts ${TS_FILES})

add_custom_target(
  lrelease
  COMMAND lrelease ${TS_FILES} -qm ${QM_FILES})

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)