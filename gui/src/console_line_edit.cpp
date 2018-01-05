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
#include "solarus/gui/console_line_edit.h"
#include "solarus/gui/settings.h"
#include <lua.hpp>
#include <QCompleter>
#include <QKeyEvent>
#include <QHash>
#include <QStringListModel>
#include <QValidator>
#include <algorithm>
#include <memory>

namespace SolarusGui {

namespace {

constexpr int max_history_size = 100;

/**
 * @brief A QValidator that checks Lua syntax.
 *
 * Only the syntax of the code is checked (via lua_load()), not its execution.
 */
class LuaSyntaxValidator : public QValidator {

public :

  explicit LuaSyntaxValidator(QObject* parent = nullptr);

  State validate(QString& input, int& pos) const override;

private:

  struct LuaStateDeleter {
      void operator()(lua_State* l) {
        lua_close(l);
      }
  };

  std::unique_ptr<lua_State, LuaStateDeleter> l;    /**< Lua context. */
  mutable QHash<QString, State> result_cache;       /**< Cache of previous results. */

};

}  // Anonymous namespace

/**
 * @brief Creates a console line field.
 * @param parent Parent object or nullptr.
 */
ConsoleLineEdit::ConsoleLineEdit(QWidget* parent) :
  QLineEdit(parent),
  history(),
  history_position(0),
  current_command() {

  Settings settings;
  history = settings.value("console_history").toStringList();

  set_history_position(history.size());  // Start after the history.

  // Set a validator.
  setValidator(new LuaSyntaxValidator(this));
  connect(this, &ConsoleLineEdit::textChanged, [this](const QString& text) {
    QString text_copy = text;
    int cursor_position = cursorPosition();
    if (validator()->validate(text_copy, cursor_position) == QValidator::Acceptable) {
      setStyleSheet("");
    }
    else {
      setStyleSheet("background-color: #ffffc0");
    }
  });

  // Set a completer.
  QStringList unique_recent_commands = history;
  unique_recent_commands.removeDuplicates();
  std::reverse(unique_recent_commands.begin(), unique_recent_commands.end());
  completer_model = new QStringListModel(unique_recent_commands, this);
  QCompleter* completer = new QCompleter(completer_model, this);
  completer->setCompletionMode(QCompleter::InlineCompletion);
  setCompleter(completer);
}

/**
 * @brief Reimplementation to navigate in the command history with up/down arrows.
 * @param event The event to handle.
 */
void ConsoleLineEdit::keyPressEvent(QKeyEvent* event) {

  switch (event->key()) {

  case Qt::Key_Up:
    history_previous();
    event->accept();
    return;

  case Qt::Key_Down:
    history_next();
    event->accept();
    return;

  }

  QLineEdit::keyPressEvent(event);
}

/**
 * @brief Shows the command at the given history position.
 * @param history_position The new history position to set.
 */
void ConsoleLineEdit::set_history_position(int history_position) {

  Q_ASSERT(history_position >= 0 && history_position <= history.size());

  this->history_position = history_position;

  if (history_position == history.size()) {
    // Typing a new command.
    setText(current_command);
  }
  else {
    // Browsing the history.
    setText(history[history_position]);
  }
}

/**
 * @brief Sets the text from the previous command in the history.
 *
 * Does nothing if there is no such command.
 */
void ConsoleLineEdit::history_previous() {

  if (history_position <= 0) {
    return;
  }

  if (history_position == history.size()) {
    // Remember the new command being typed.
    current_command = text();
  }

  set_history_position(history_position - 1);
}

/**
 * @brief Sets the text from the next command in the history.
 *
 * Does nothing if there is no such command.
 */
void ConsoleLineEdit::history_next() {

  if (history_position >= history.size()) {
    return;
  }

  set_history_position(history_position + 1);
}

/**
 * @brief Adds a command to the history.
 * @param command The command just executed.
 */
void ConsoleLineEdit::command_executed(const QString& command) {

  // Avoid consecutive duplicates.
  if (history.size() == 0 || command != history.last()) {
    history.append(command);

    // Limit the history size.
    while (history.size() > max_history_size) {
      history.removeFirst();
    }

    // Save the modified history.
    Settings settings;
    settings.setValue("console_history", history);
  }

  // Update the history position.
  current_command.clear();
  set_history_position(history.size());

  // Update the completer.
  if (!completer_model->stringList().contains(command)) {
    completer_model->insertRow(0);
    QModelIndex index = completer_model->index(0, 0);
    completer_model->setData(index, command);
  }
}

/**
 * @brief Creates a Lua syntax validator.
 * @param parent Parent object or nullptr.
 */
LuaSyntaxValidator::LuaSyntaxValidator(QObject* parent) :
  QValidator(parent),
  l(luaL_newstate()) {

}

/**
 * @brief Validates the Lua syntax of an input.
 * @param input The input to check.
 * @param pos The cursor position.
 * @return The syntax validity: Acceptable or Intermediate.
 */
QValidator::State LuaSyntaxValidator::validate(QString& input, int& pos) const {

  Q_UNUSED(pos);

  const auto& it = result_cache.find(input);
  if (it != result_cache.end()) {
    // We already know the result.
    return it.value();
  }

  // New input: ask Lua.
  QByteArray input_utf8 = input.toUtf8();
  const int load_result = luaL_loadstring(l.get(), input_utf8.data());

  State result = (load_result == 0) ? State::Acceptable : State::Intermediate;
  result_cache.insert(input, result);
  return result;
}

}
