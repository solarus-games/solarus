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
set(solarus_gui_SOURCES
  include/gui/main_window.h
  src/gui/main_window.cpp
  src/main.cpp
)

# Add an icon for the executable in Windows.
if(WIN32)
  set(solarus_gui_SOURCES
    ${solarus_gui_SOURCES}
    ../cmake/win32/resources.rc
  )
endif()

# UI files.
set(solarus_gui_FORMS
  src/gui/main_window.ui
)

# Generate .h from .ui.
qt5_wrap_ui(solarus_gui_FORMS_HEADERS
  ${solarus_gui_FORMS}
)

# Resources.
set(solarus_gui_RESOURCES
)

# Compile resource files.
qt5_add_resources(solarus_gui_RESOURCES_RCC
  ${solarus_gui_RESOURCES}
)

# Translations.
set(solarus_gui_TRANSLATIONS
)
qt5_add_translation(solarus_gui_TRANSLATIONS_QM
  ${solarus_gui_TRANSLATIONS}
)

# Main executable.
add_executable(solarus-gui
  ${solarus_gui_SOURCES}
  ${solarus_gui_FORMS_HEADERS}
  ${solarus_gui_RESOURCES_RCC}
  ${solarus_gui_TRANSLATIONS_QM}
)

target_link_libraries(solarus-gui
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

# Name the GUI executable "solarus", not "solarus-gui".
# "solarus-gui" is the cmake target name because
# the "solarus" cmake target name is already used by the library.
set_target_properties(solarus-gui PROPERTIES
  OUTPUT_NAME solarus
)

# Set files to install
install(TARGETS solarus-gui
  RUNTIME DESTINATION bin
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

