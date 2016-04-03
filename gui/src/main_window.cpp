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
#include "solarus/gui/gui_tools.h"
#include "solarus/gui/main_window.h"
#include "solarus/gui/quests_view.h"
#include "solarus/gui/settings.h"
#include "solarus/gui/about_dialog.h"
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
  QStringList icon_sizes = { "16", "24", "32", "48", "64", "128", "256" };
  QIcon icon;
  for (const QString size : icon_sizes) {
    icon.addPixmap(":/images/icon/solarus_launcher_icon_" + size + ".png");
  }
  setWindowIcon(icon);

  // Console.
  ui.console->set_quest_runner(quest_runner);
  ui.console->set_command_enabled(false);
  const int console_height = 150;
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

  // Menus.
  initialize_menus();

  // Actions.
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
  connect(ui.console, SIGNAL(setting_changed_in_quest(QString, QVariant)),
          this, SLOT(setting_changed_in_quest(QString, QVariant)));

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
 * @brief Sets up the menus of the main window.
 */
void MainWindow::initialize_menus() {

  QActionGroup* group = new QActionGroup(this);

  const QMap<QAction*, QString> video_mode_names = {
    { ui.action_filter_normal, "normal" },
    { ui.action_filter_scale2x, "scale2x" },
    { ui.action_filter_hq2x, "hq2x" },
    { ui.action_filter_hq3x, "hq3x" },
    { ui.action_filter_hq4x, "hq4x" }
  };

  for (auto it = video_mode_names.constBegin(); it != video_mode_names.constEnd(); ++it) {
    QAction* action = it.key();
    const QString& name = it.value();
    group->addAction(action);
    action->setData(name);
    connect(action, &QAction::triggered, [=]() {
      set_video_mode_requested(name);
    });
  }

  // TODO implement the audio menu
  delete ui.menu_audio;
  ui.menu_audio = nullptr;

  update_menus();
}

/**
 * @brief Updates menus with the current settings.
 */
void MainWindow::update_menus() {

  update_filter_menu();
  update_fullscreen_action();
  update_video_acceleration_action();
}

/**
 * @brief Updates the video filter menu with the current settings.
 */
void MainWindow::update_filter_menu() {

  Settings settings;

  QString video_mode = settings.value("quest_video_mode", "normal").toString();

  if (video_mode == "normal") {
    ui.action_filter_normal->setChecked(true);
  }
  else if (video_mode == "scale2x") {
    ui.action_filter_scale2x->setChecked(true);
  }
  else if (video_mode == "hq2x") {
    ui.action_filter_hq2x->setChecked(true);
  }
  else if (video_mode == "hq3x") {
    ui.action_filter_hq3x->setChecked(true);
  }
  else if (video_mode == "hq4x") {
    ui.action_filter_hq4x->setChecked(true);
  }
  else {
    ui.action_filter_normal->setChecked(true);
    settings.setValue("quest_video_mode", "normal");
  }
}

/**
 * @brief Updates the fullscreen action with the current settings.
 */
void MainWindow::update_fullscreen_action() {

  Settings settings;

  bool fullscreen = settings.value("quest_fullscreen", false).toBool();
  ui.action_fullscreen->setChecked(fullscreen);
}

/**
 * @brief Updates the 2D acceleration action with the current settings.
 */
void MainWindow::update_video_acceleration_action() {

  Settings settings;

  bool video_acceleration = settings.value("video_acceleration", true).toBool();
  ui.action_video_acceleration->setChecked(video_acceleration);
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

  QMessageBox::StandardButton answer = QMessageBox::warning(
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

  if (ui.quests_view->has_quest(quest_path)) {
    // Quest already in the list.
    ui.quests_view->select_quest(quest_path);
    return;
  }

  // Add to the quest list view.
  if (!ui.quests_view->add_quest(quest_path)) {
    GuiTools::error_dialog("No quest was found in this directory");
    return;
  }

  // Remember the quest list.
  Settings settings;
  settings.setValue("quests_paths", ui.quests_view->get_paths());

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

  // Write system settings to the settings.dat file of the quest.
  Settings settings;
  settings.export_to_quest(path);

  // Run the quest.
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
 * @brief Slot called when the user triggers the "Fullscreen" action.
 */
void MainWindow::on_action_fullscreen_triggered() {

  bool fullscreen = ui.action_fullscreen->isChecked();

  Settings settings;
  bool previous = settings.value("quest_fullscreen", false).toBool();
  if (fullscreen == previous) {
    return;
  }

  settings.setValue("quest_fullscreen", fullscreen);

  if (quest_runner.is_started()) {
    // Change the setting in the current quest process.
    QString command = QString("sol.video.set_fullscreen(%1)").arg(fullscreen ? "true" : "false");
    ui.console->execute_command(command);
  }
}

/**
 * @brief Slot called when the user triggers the "2D acceleration" action.
 */
void MainWindow::on_action_video_acceleration_triggered() {

  bool video_acceleration = ui.action_video_acceleration->isChecked();

  Settings settings;
  bool previous = settings.value("video_acceleration", true).toBool();
  if (video_acceleration == previous) {
    return;
  }

  settings.setValue("video_acceleration", video_acceleration);

  if (quest_runner.is_started()) {
    QMessageBox::information(
          this,
          tr("2D acceleration changed"),
          tr("The change will take effect next time you play a quest."),
          QMessageBox::Ok
    );
  }
}

/**
 * @brief Slot called when the user triggers the "Zoom x1" action.
 */
void MainWindow::on_action_zoom_x1_triggered() {
  set_zoom_requested(1);
}

/**
 * @brief Slot called when the user triggers the "Zoom x2" action.
 */
void MainWindow::on_action_zoom_x2_triggered() {
  set_zoom_requested(2);
}

/**
 * @brief Slot called when the user triggers the "Zoom x3" action.
 */
void MainWindow::on_action_zoom_x3_triggered() {
  set_zoom_requested(3);
}

/**
 * @brief Slot called when the user triggers the "Zoom x4" action.
 */
void MainWindow::on_action_zoom_x4_triggered() {
  set_zoom_requested(4);
}

/**
 * @brief Slot called when the user triggers the "About" action.
 */
void MainWindow::on_action_about_triggered() {
  AboutDialog dialog(this);
  dialog.exec();
}

/**
 * @brief Slot called when the quest has just started or stopped.
 */
void MainWindow::update_run_quest() {

  QString selected_path = ui.quests_view->get_selected_path();
  bool has_current = !selected_path.isEmpty();
  bool playing = quest_runner.is_started();

  bool enable_play = has_current && !playing;
  bool enable_stop = has_current && playing;
  ui.action_play_quest->setEnabled(enable_play);
  ui.play_button->setEnabled(enable_play);
  ui.action_stop_quest->setEnabled(enable_stop);

  ui.menu_zoom->setEnabled(playing);
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

  static const QPixmap default_pixmap = QPixmap(":/images/no_logo.png");

  if (!has_current) {
    ui.quest_info_panel->setEnabled(false);

    ui.quest_title_value->clear();
    ui.quest_title_value->setVisible(false);

    ui.quest_author_label->setVisible(false);
    ui.quest_author_value->setVisible(false);
    ui.quest_author_value->clear();

    ui.quest_version_label->setVisible(false);
    ui.quest_version_value->setVisible(false);
    ui.quest_version_value->clear();

    ui.quest_long_description_value->setVisible(false);
    ui.quest_long_description_value->clear();

    ui.quest_box_label->setPixmap(default_pixmap);
  }
  else {
    ui.quest_info_panel->setEnabled(true);

    const Solarus::QuestProperties properties =
            ui.quests_view->get_selected_quest_properties();

    ui.quest_box_label->setPixmap(default_pixmap);
    //ui.quest_box_label->setText("ayon");

    QString title = QString::fromStdString(properties.get_title());
    ui.quest_title_value->setVisible(true);
    ui.quest_title_value->setText(title);

    QString author = QString::fromStdString(properties.get_author());
    if (author.isEmpty()) {
      ui.quest_author_label->setVisible(false);
      ui.quest_author_value->setVisible(false);
    }
    else {
      ui.quest_author_label->setVisible(true);
      ui.quest_author_value->setText(author);
      ui.quest_author_value->setVisible(true);
    }

    QString quest_version =
            QString::fromStdString(properties.get_quest_version());
    if (quest_version.isEmpty()) {
      ui.quest_version_label->setVisible(false);
      ui.quest_version_value->setVisible(false);
    }
    else {
      ui.quest_version_label->setVisible(true);
      ui.quest_version_value->setText(quest_version);
      ui.quest_version_value->setVisible(true);
    }

    QString long_description =
            QString::fromStdString(properties.get_long_description());
    ui.quest_long_description_value->setVisible(true);
    ui.quest_long_description_value->setText(long_description);
  }
}

/**
 * @brief Slot called when a setting changes in the currently running quest.
 *
 * The GUI is updated to reflect and remember the new setting.
 *
 * @param key Name of the system setting that has just changed.
 * @param value The new value.
 */
void MainWindow::setting_changed_in_quest(
    const QString& key, const QVariant& value
) {

  Settings settings;
  if (key == "quest_video_mode") {
    settings.setValue(key, value);
    update_filter_menu();
  }
  else if (key == "quest_fullscreen") {
    settings.setValue(key, value);
    update_fullscreen_action();
  }
}

/**
 * @brief Slot called when the user wants to change the video mode.
 * @param video_mode_name The new video mode.
 */
void MainWindow::set_video_mode_requested(const QString& video_mode_name) {

  Settings settings;
  const QString& previous = settings.value("quest_video_mode").toString();
  if (video_mode_name == previous) {
    return;
  }

  settings.setValue("quest_video_mode", video_mode_name);

  if (quest_runner.is_started()) {
    // Change the video mode of the current quest process.
    QString command = QString("sol.video.set_mode(\"%1\")").arg(video_mode_name);
    ui.console->execute_command(command);
  }
}

/**
 * @brief Slot called when the user wants to change the window zoom.
 * @param zoom The new zoom factor.
 */
void MainWindow::set_zoom_requested(int zoom) {

  if (!quest_runner.is_started()) {
    return;
  }

  QString command = QString("local w, h = sol.video.get_quest_size(); sol.video.set_window_size(w * %1, h * %2)").arg(zoom).arg(zoom);
  ui.console->execute_command(command);
}

}
