# As Qt generates ui header files, the build directory needs to be an include
# directory too.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Tell CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Source files.
set(solarus_gui_lib_SOURCES
  include/solarus/gui/about_dialog.h
  include/solarus/gui/console.h
  include/solarus/gui/console_line_edit.h
  include/solarus/gui/gui_common.h
  include/solarus/gui/gui_tools.h
  include/solarus/gui/quest_runner.h
  include/solarus/gui/quests_item_delegate.h
  include/solarus/gui/quests_model.h
  include/solarus/gui/quests_view.h
  include/solarus/gui/main_window.h
  include/solarus/gui/settings.h
  src/about_dialog.cpp
  src/console.cpp
  src/console_line_edit.cpp
  src/gui_tools.cpp
  src/quest_runner.cpp
  src/quests_item_delegate.cpp
  src/quests_model.cpp
  src/quests_view.cpp
  src/main_window.cpp
  src/settings.cpp
)

# UI files.
set(solarus_gui_lib_FORMS
  src/about_dialog.ui
  src/console.ui
  src/main_window.ui
)

# Generate .h from .ui.
qt5_wrap_ui(solarus_gui_lib_FORMS_HEADERS
  ${solarus_gui_lib_FORMS}
)

# Resources.
set(solarus_gui_lib_RESOURCES
  resources/images.qrc
)

# Compile resource files.
qt5_add_resources(solarus_gui_lib_RESOURCES_RCC
  ${solarus_gui_lib_RESOURCES}
)

# Translations.
set(solarus_gui_lib_TRANSLATIONS
  translations/solarus_es.ts
  translations/solarus_fr.ts
)
qt5_add_translation(solarus_gui_lib_TRANSLATIONS_QM
  ${solarus_gui_lib_TRANSLATIONS}
)

# Build the Solarus GUI library.
add_library(solarus_gui_lib
  SHARED
  ${solarus_gui_lib_SOURCES}
  ${solarus_gui_lib_FORMS_HEADERS}
  ${solarus_gui_lib_RESOURCES_RCC}
  ${solarus_gui_lib_TRANSLATIONS_QM}
)

target_link_libraries(solarus_gui_lib
  solarus
  Qt5::Widgets
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
)

set_target_properties(solarus_gui_lib PROPERTIES
  OUTPUT_NAME solarus-gui
)
# Main executable.
set(solarus_gui_exe_SOURCES
  src/main.cpp
)

# Add the Solarus icon in Windows.
if(WIN32)
  set(solarus_gui_exe_SOURCES
    ${solarus_gui_exe_SOURCES}
    ../gui/resources/win32/resources.rc
  )
endif()

add_executable(solarus_gui_exe
  ${solarus_gui_exe_SOURCES}
)

target_link_libraries(solarus_gui_exe
  solarus
  solarus_gui_lib
  Qt5::Widgets
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
)

# Name the GUI executable "solarus".
set_target_properties(solarus_gui_exe PROPERTIES
  OUTPUT_NAME solarus
)

# Set files to install.
install(TARGETS solarus_gui_lib solarus_gui_exe
  LIBRARY DESTINATION ${SOLARUS_LIBRARY_INSTALL_DESTINATION}
  RUNTIME DESTINATION ${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}
)
install(DIRECTORY
  "${CMAKE_CURRENT_SOURCE_DIR}/include/solarus"
  DESTINATION ${SOLARUS_HEADERS_INSTALL_DESTINATION}
)
install(FILES
  ${solarus_gui_lib_FORMS_HEADERS}
  DESTINATION "${SOLARUS_HEADERS_INSTALL_DESTINATION}/solarus/gui"
)

# Platform specific.

# Windows: disable the console.
if(WIN32)
  if(MSVC)
    set_target_properties(solarus-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-mwindows ${CMAKE_CXX_FLAGS}")
  endif()
endif()

# FreeDesktop compatible icons
if(UNIX AND NOT APPLE)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_16.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_24.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/24x24/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_32.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_48.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_64.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_128.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_256.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps RENAME solarus.png)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/icons/solarus_icon_512.png
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps RENAME solarus.png)
endif(UNIX AND NOT APPLE)

# FreeDesktop compatible start menu launcher
if(UNIX AND NOT APPLE)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/solarus.desktop
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif(UNIX AND NOT APPLE)

# Linux Manpage
if(UNIX AND NOT APPLE)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../solarus.6
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man6)
endif(UNIX AND NOT APPLE)
