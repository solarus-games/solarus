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
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>

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

  // Select the last quest played.
  QString last_quest = settings.value("last_quest").toString();
  if (!last_quest.isEmpty()) {
    ui.quests_view->select_quest(last_quest);
  }

  ui.action_add_quest->setShortcut(QKeySequence::Open);
  ui.action_exit->setShortcut(QKeySequence::Quit);
  ui.add_button->setDefaultAction(ui.action_add_quest);
  ui.remove_button->setDefaultAction(ui.action_remove_quest);

  // Make connections.
  connect(ui.quests_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
          this, SLOT(selected_quest_changed()));
  connect(ui.play_button, SIGNAL(clicked()),
          this, SLOT(on_action_play_quest_triggered()));
  connect(&quest_runner, SIGNAL(running()),
          this, SLOT(update_run_quest()));
  connect(&quest_runner, SIGNAL(finished()),
          this, SLOT(update_run_quest()));

  selected_quest_changed();
  update_run_quest();
}

/**
 * @brief Sets an appropriate size and centers the window on the screen having
 * the mouse.
 */
void MainWindow::initialize_geometry_on_screen() {

  QDesktopWidget* desktop = QApplication::desktop();
  QRect screen = desktop->screenGeometry(desktop->screenNumber(QCursor::pos()));

  // Center the window on the screen where the mouse is currently.
  int x = screen.width() / 2 - frameGeometry().width() / 2 + screen.left() - 2;
  int y = screen.height() / 2 - frameGeometry().height() / 2 + screen.top() - 10;

  move(qMax(0, x), qMax(0, y));
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
 * @brief Receives a window close event.
 * @param event The event to handle.
 */
void MainWindow::closeEvent(QCloseEvent* event) {

  if (confirm_close()) {
    event->accept();
  }
  else {
    event->ignore();
  }
}

/**
 * @brief Function called when the user wants to exit the program.
 *
 * A confirmation dialog is shown if a quest is running.
 *
 * @return @c false to cancel the closing operation.
 */
bool MainWindow::confirm_close() {

  if (!quest_runner.is_started()) {
    // No quest is running.
    return true;
  }

  QMessageBox::StandardButton answer = QMessageBox::question(
        nullptr,
        tr("A quest is playing"),
        tr("A quest is playing. Do you really want to exit Solarus?"),
        QMessageBox::Close | QMessageBox::Cancel
  );

  switch (answer) {

  case QMessageBox::Close:
    return true;


  case QMessageBox::Cancel:
  case QMessageBox::Escape:
    return false;

  default:
    return false;
  }

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

  // Select it.
  ui.quests_view->select_quest(quest_path);
}

/**
 * @brief Slot called when the user removes a quest to the quest list.
 */
void MainWindow::on_action_remove_quest_triggered() {

  int selected_index = ui.quests_view->get_selected_index();

  if (selected_index == -1) {
    return;
  }

  // Remove from the quest list view.
  if (ui.quests_view->remove_quest(selected_index)) {

    // Update the quest list.
    Settings settings;
    settings.setValue("quests_paths", ui.quests_view->get_paths());
  }

  // Select the next one.
  int num_quests = ui.quests_view->get_num_quests();
  selected_index = qMin(selected_index, num_quests - 1);
  ui.quests_view->select_quest(selected_index);
}

/**
 * @brief Slot called when the user triggers the "Exit" action.
 */
void MainWindow::on_action_exit_triggered() {

  if (confirm_close()) {
    QApplication::exit(0);
  }
}

/**
 * @brief Slot called when the user triggers the "Play quest" action.
 */
void MainWindow::on_action_play_quest_triggered() {

  if (quest_runner.is_started()) {
    return;
  }

  QString path = ui.quests_view->get_selected_path();
  if (path.isEmpty()) {
    return;
  }
  quest_runner.start(path);

  update_run_quest();
}

/**
 * @brief Slot called when the user triggers the "Stop quest" action.
 */
void MainWindow::on_action_stop_quest_triggered() {

  if (!quest_runner.is_started()) {
    return;
  }

  quest_runner.stop();

  update_run_quest();
}

/**
 * @brief Slot called when the quest has just started or stopped.
 */
void MainWindow::update_run_quest() {

  QString selected_path = ui.quests_view->get_selected_path();
  bool has_current = !selected_path.isEmpty();

  bool enable_play = has_current && !quest_runner.is_started();
  bool enable_stop = has_current && quest_runner.is_started();
  ui.action_play_quest->setEnabled(enable_play);
  ui.play_button->setEnabled(enable_play);
  ui.action_stop_quest->setEnabled(enable_stop);
}

/**
 * @brief Slot called when the selection changes in the quest list.
 */
void MainWindow::selected_quest_changed() {

  QString selected_path = ui.quests_view->get_selected_path();
  bool has_current = !selected_path.isEmpty();

  // Enable/disable buttons.
  ui.action_remove_quest->setEnabled(has_current);
  update_run_quest();

  // Save the last selected quest (including if none).
  Settings settings;
  settings.setValue("last_quest", selected_path);
}

}
