/*
 * Copyright (C) 2014-2016 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "solarus/gui/main_window.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/Arguments.h"
#include "solarus/MainLoop.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QTranslator>

namespace SolarusGui {

namespace {

/**
 * @brief Runs the Solarus GUI.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int run_gui(int argc, char* argv[]) {

  // Set up the application.
  QApplication application(argc, argv);
  application.setApplicationName("Solarus");
  application.setApplicationVersion(SOLARUS_VERSION);
  application.setOrganizationName("Solarus");

  // Set up the translations.
  QTranslator qt_translator;
  qt_translator.load("qt_" + QLocale::system().name(),
                     QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  application.installTranslator(&qt_translator);

  QTranslator translator;
  translator.load("solarus_" + QLocale::system().name());
  application.installTranslator(&translator);

  MainWindow window(nullptr);
  window.initialize_geometry_on_screen();
  window.show();

  return application.exec();
}

/**
 * @brief Runs a quest like the solarus-run executable does.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int run_quest(int argc, char* argv[]) {

  // Abort the process in case of fatal error.
  Solarus::Debug::set_abort_on_die(true);

  // Run the Solarus main loop.
  const Solarus::Arguments args(argc, argv);
  Solarus::MainLoop(args).run();

  return 0;
}

}  // Anonymous namespace

}  // namespace SolarusGui

/**
 * @brief Entry point of Solarus.
 *
 * To run the Solarus GUI:
 *   solarus
 * To directly run a quest (no GUI, similar to solarus-run):
 *   solarus -run quest_path
 *
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int main(int argc, char* argv[]) {

  if (argc > 1 && QString(argv[1]) == "-run") {
    // Quest run mode.
    return SolarusGui::run_quest(argc, argv);
  }
  else {
    // Solarus GUI mode.
    return SolarusGui::run_gui(argc, argv);
  }
}
