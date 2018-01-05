/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_GUI_QUEST_RUNNER_H
#define SOLARUS_GUI_QUEST_RUNNER_H

#include "solarus/gui/gui_common.h"
#include <QProcess>

namespace SolarusGui {

/**
 * @brief Class to run a quest in a dedicated process.
 */
class SOLARUS_GUI_API QuestRunner : public QObject {
  Q_OBJECT

public:

  QuestRunner(QObject* parent = nullptr);
  ~QuestRunner();

  bool is_started() const;
  bool is_running() const;
  int execute_command(const QString& command);
  bool apply_settings();

public slots:

  void start(const QString& quest_path);
  void stop();

signals:

  void running();
  void finished();
  void solarus_fatal(const QString& what);
  void output_produced(const QStringList& lines);

private slots:

  void standard_output_data_available();
  void on_finished();

private:

  QStringList create_arguments(const QString& quest_path) const;
  QStringList get_quest_lua_commands_from_settings() const;

  QProcess process;     /**< The Solarus process. */
  int last_command_id;  /**< Id of the last command executed (-1 if none). */
};

}

#endif
