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

add_qrc_files(resources.qrc)

enable_localized_languages(en de)
set_ts_files_directory(translations)