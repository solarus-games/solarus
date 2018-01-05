/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/gui/settings.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/Arguments.h"
#include "solarus/MainLoop.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QFileInfo>
#include <QMainWindow>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QTextStream>
#include <QTranslator>
#include <iostream>

namespace SolarusGui {

namespace {

/**
 * @brief Prints the command-line usage.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int print_help(int argc, char* argv[]) {

  std::string binary_name = (argc > 0) ? argv[0] : "solarus";
  std::cout << "Usage:\n" <<
               "  " << binary_name << "                         Opens the Solarus launcher GUI\n" <<
               "  " << binary_name << " -help                   Prints this help message and exits\n" <<
               "  " << binary_name << " -run [/path/to/quest]   Plays a quest (default: " << SOLARUS_DEFAULT_QUEST << ")\n" <<
               "  " << binary_name << " -add /path/to/quest     Adds a quest to the launcher's list and exits\n";
  return 0;
}

/**
 * @brief Runs the Solarus GUI.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int run_gui(int argc, char* argv[]) {

  // Set up the application.
  QApplication application(argc, argv);
  application.setApplicationName("solarus");
  application.setApplicationVersion(SOLARUS_VERSION);
  application.setOrganizationName("solarus");

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

/**
 * @brief Adds a quest to the list of quest if it is not already there.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int add_quest(int argc, char* argv[]) {

  if (argc <= 2) {
    std::cerr << "Usage: " << ((argc > 0) ? argv[0] : "solarus") << " -add /path/to/quest" << std::endl;
    return 1;
  }

  // Set up the application.
  QApplication application(argc, argv);
  application.setApplicationName("solarus");
  application.setApplicationVersion(SOLARUS_VERSION);
  application.setOrganizationName("solarus");

  QString new_path = argv[2];
  const QString& canonical_path = QFileInfo(new_path).canonicalFilePath();
  if (!QFile(canonical_path + "/data/quest.dat").exists() &&
      !QFile(canonical_path + "/data.solarus").exists() &&
      !QFile(canonical_path + "/data.solarus.zip").exists()) {
    std::cerr << "Not a valid Solarus quest: " << new_path.toStdString() << std::endl;
    return 1;
  }

  // Add the new path if not already present.
  Settings test_settings(QSettings::SystemScope);
  const bool system_settings_writable = test_settings.isWritable();

  Settings settings(system_settings_writable ? QSettings::SystemScope : QSettings::UserScope);

  QStringList all_paths = settings.value("quests_paths").toStringList();
  for (const QString& path : all_paths) {
    if (QFileInfo(path).canonicalFilePath() == canonical_path) {
      std::cout << "Nothing to do, quest already registered: " << new_path.toStdString() << std::endl;
      return 0;
    }
  }

  if (!settings.isWritable()) {
    std::cerr << "Settings file is not writable: " << settings.fileName().toStdString() << std::endl;
    return 1;
  }
  std::cout << "Writing settings file " << settings.fileName().toStdString() << std::endl;

  all_paths.prepend(canonical_path);
  settings.setValue("quests_paths", all_paths);
  settings.setValue("last_quest", new_path);
  std::cout << "Quest successfully added to Solarus: " << new_path.toStdString() << std::endl;

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
 * To silently add a quest to the Solarus GUI quest list and exit immediately
 * (no GUI is run):
 *   solarus -add quest_path
 *
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int main(int argc, char* argv[]) {

  if (argc > 1) {
    const QString& arg1 = argv[1];
    if (arg1 == "-help") {
      return SolarusGui::print_help(argc, argv);
    }
    else if (arg1 == "-run") {
      // Quest run mode.
      return SolarusGui::run_quest(argc, argv);
    }
    else if (arg1 == "-add") {
      return SolarusGui::add_quest(argc, argv);
    }
  }

  // Solarus GUI mode.
  return SolarusGui::run_gui(argc, argv);
}
