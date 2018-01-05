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
#ifndef SOLARUS_GUI_MAIN_WINDOW_H
#define SOLARUS_GUI_MAIN_WINDOW_H

#include "solarus/gui/gui_common.h"
#include "solarus/gui/quest_runner.h"
#include "ui_main_window.h"

namespace SolarusGui {

/**
 * @brief Main window of the Solarus GUI.
 */
class SOLARUS_GUI_API MainWindow : public QMainWindow {
  Q_OBJECT

public:

  explicit MainWindow(QWidget* parent = nullptr);

  void initialize_geometry_on_screen();

protected:

  void closeEvent(QCloseEvent* event) override;

private slots:

  void on_action_add_quest_triggered();
  void on_action_remove_quest_triggered();
  void on_action_exit_triggered();
  void on_action_play_quest_triggered();
  void on_action_stop_quest_triggered();
  void on_action_fullscreen_triggered();
  void on_action_zoom_x1_triggered();
  void on_action_zoom_x2_triggered();
  void on_action_zoom_x3_triggered();
  void on_action_zoom_x4_triggered();
  void on_action_about_triggered();

  void selected_quest_changed();
  void update_run_quest();

  void setting_changed_in_quest(const QString& key, const QVariant& value);
  void set_zoom_requested(int zoom);

private:

  void update_title();
  bool confirm_close();
  void initialize_menus();
  void update_menus();
  void update_fullscreen_action();

  Ui::MainWindow ui;         /**< The widgets. */
  QuestRunner quest_runner;  /**< The quest executor. */

};

}

#endif
