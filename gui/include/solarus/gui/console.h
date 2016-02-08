/*
 * Copyright (C) 2014-2016 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_GUI_CONSOLE_H
#define SOLARUS_GUI_CONSOLE_H

#include "solarus/Common.h"
#include "ui_console.h"
#include <QPointer>

namespace SolarusGui {

class QuestRunner;

/**
 * @brief Quest execution log viewer and interactive console to execute Lua
 * commands.
 */
class SOLARUS_API Console : public QWidget {
  Q_OBJECT

public:

  explicit Console(QWidget* parent = nullptr);

  void set_quest_runner(QuestRunner& quest_runner);

  bool is_command_enabled() const;
  void set_command_enabled(bool enable);
  bool execute_command(const QString& command);

signals:

  void setting_changed_in_quest(const QString& key, const QVariant& value);

private slots:

  void quest_running();
  void quest_finished();
  void quest_output_produced(const QStringList& lines);
  void command_field_activated();

private:

  void parse_output(const QString& line);
  void detect_setting_change(const QString& log_level, const QString& message);
  QString colorize_output(const QString& log_level, const QString& message);

  Ui::Console ui;                      /**< The widgets. */
  QPointer<QuestRunner> quest_runner;  /**< The quest execution. */
  bool command_enabled_;               /**< Whether the user can type Lua commands. */

};

}

#endif
