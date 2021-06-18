begin_project(QtMinesweeper EXECUTABLE)

enable_automoc()
enable_autorcc()
enable_autouic()

require_library(Qt6 MODULES Core Gui Widgets)
require_library(Qt6LinguistTools)

add_source_directory(src)
add_source_directory(include)

add_include_directory(include)

add_ui_directory(ui)

add_ts_directory(translations)

add_qrc_files(resources.qrc)



#find_package(Qt5LinguistTools REQUIRED)



#set(TS_FILES
#  ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_en.ts
#  ${CMAKE_CURRENT_SOURCE_DIR}/translations/${PROJECT_NAME}_de.ts)

#qt5_add_translation(QM_FILES ${TS_FILES})

#add_executable(${PROJECT_NAME}
#  ${SOURCE_FILES}
#  ${HEADER_FILES}
#  ${UI_FILES}
#  ${QM_FILES}
#  ${TS_FILES}
#  ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)

#source_group(translations ${QM_FILES} ${TS_FILES})


#add_custom_target(
#  lupdate
#  COMMAND lupdate -I ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src -ts ${TS_FILES})

#add_custom_target(
#  lrelease
#  COMMAND lrelease ${TS_FILES} -qm ${QM_FILES})
