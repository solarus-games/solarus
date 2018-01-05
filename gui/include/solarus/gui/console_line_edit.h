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
#ifndef SOLARUS_GUI_CONSOLE_LINE_EDIT_H
#define SOLARUS_GUI_CONSOLE_LINE_EDIT_H

#include "solarus/gui/gui_common.h"
#include <QLineEdit>

class QStringListModel;

namespace SolarusGui {

/**
 * @brief A line field to execute Lua commands to the quest.
 *
 * Manages the history of commands.
 */
class SOLARUS_GUI_API ConsoleLineEdit : public QLineEdit {
  Q_OBJECT

public:

  explicit ConsoleLineEdit(QWidget* parent = nullptr);

public slots:

  void set_history_position(int history_position);
  void history_previous();
  void history_next();
  void command_executed(const QString& command);

protected:

  void keyPressEvent(QKeyEvent* event) override;

private:

  QStringList history;       /**< Commands history. */
  int history_position;      /**< Current position in the history, between
                              * 0 and history.size() - 1, or
                              * history.size() if not navigating the history. */
  QString current_command;   /**< Command currently being edited, preserved
                              * while navigating the history. */
  QStringListModel*
      completer_model;       /**< Model for the completer. */

};

}

#endif
