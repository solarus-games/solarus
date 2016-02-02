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
#include "solarus/gui/quests_view.h"
#include "solarus/gui/settings.h"
#include "solarus/Common.h"
#include <QFileDialog>

namespace SolarusGui {

/**
 * @brief Creates a main window.
 * @param parent Parent object or nullptr.
 */
MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  quest_runner() {

  // Set up widgets.
  ui.setupUi(this);

  // Title.
  update_title();

  // Icon.
  setWindowIcon(QIcon(":/images/icon_solarus.png"));  // TODO different sizes

  // Console.
  ui.console->set_quest_runner(quest_runner);
  ui.console->set_command_enabled(false);
  const int console_height = 100;
  ui.console_splitter->setSizes({ height() - console_height, console_height });

  // Show recent quests.
  Settings settings;
  QStringList quest_paths = settings.value("quests_paths").toStringList();
  for (const QString& path : quest_paths) {
    ui.quests_view->add_quest(path);
  }
}

/**
 * @brief Sets the title of the window.
 */
void MainWindow::update_title() {

  QString version = SOLARUS_VERSION;
  QString title = tr("Solarus %1").arg(version);
  setWindowTitle(title);
}

/**
 * @brief Function called when the user wants to exit the program.
 *
 * A confirmation dialog is shown if a quest is running.
 *
 * @return @c false to cancel the closing operation.
 */
bool MainWindow::confirm_close() {

  // TODO confirmation dialog if a quest is running
  return true;
}

/**
 * @brief Slot called when the user adds a quest to the quest list.
 */
void MainWindow::on_action_add_quest_triggered() {

  QString quest_path = QFileDialog::getExistingDirectory(
        this,
        tr("Select quest directory")
  );

  if (quest_path.isEmpty()) {
    return;
  }

  // Add to the quest list view.
  if (ui.quests_view->add_quest(quest_path)) {

    // Remember the quest list.
    Settings settings;
    settings.setValue("quests_paths", ui.quests_view->get_paths());
  }
}

/**
 * @brief Slot called when the user triggers the "Exit" action.
 */
void MainWindow::on_action_exit_triggered() {

  if (confirm_close()) {
    QApplication::exit(0);
  }
}

}
