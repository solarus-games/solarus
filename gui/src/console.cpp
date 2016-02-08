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
#include "solarus/gui/console.h"
#include "solarus/gui/quest_runner.h"
#include <QRegularExpression>

namespace SolarusGui {

namespace {

/**
 * @brief Wraps a line of plain text in html color tags.
 * @param line A plain text line.
 * @param color The color to set, with "#rrggbb" syntax.
 * @return The HTML colorized line.
 */
QString colorize(const QString& line, const QString& color) {

  return QString("<span style=\"color: %1\">%2</span>").arg(color, line.toHtmlEscaped());
}

const QRegularExpression output_regexp("^\\[Solarus\\] \\[(\\d+)\\] (\\w+): (.+)$");
const QRegularExpression output_simplify_console_error_regexp("In Lua command: \\[string \".*\"\\]:\\d+: ");
const QRegularExpression output_setting_video_mode_regexp("^Video mode: (\\w+)$");
const QRegularExpression output_setting_fullscreen_regexp("^Fullscreen: (\\w+)$");

}

/**
 * @brief Creates a console view.
 * @param parent Parent object or nullptr.
 */
Console::Console(QWidget* parent) :
  QWidget(parent),
  command_enabled_(true) {

  ui.setupUi(this);
}

/**
 * @brief Returns whether the Lua command input field is available.
 * @return @c true if the user can type Lua commands.
 */
bool Console::is_command_enabled() const {
  return command_enabled_;
}

/**
 * @brief Sets whether the Lua command input field is available.
 * @param enable @c true to allow user to type Lua commands.
 */
void Console::set_command_enabled(bool enable) {

  this->command_enabled_ = enable;
  ui.command_field->setVisible(enable);
}

/**
 * @brief Connects this console to a quest runner.
 * @param quest_runner The quest runner.
 */
void Console::set_quest_runner(QuestRunner& quest_runner) {

  this->quest_runner = &quest_runner;

  connect(ui.command_field, SIGNAL(returnPressed()),
          this, SLOT(command_field_activated()));

  connect(&quest_runner, SIGNAL(running()),
          this, SLOT(quest_running()));
  connect(&quest_runner, SIGNAL(finished()),
          this, SLOT(quest_finished()));
  connect(&quest_runner, SIGNAL(output_produced(QStringList)),
          this, SLOT(quest_output_produced(QStringList)));

}

/**
 * @brief Slot called when the quest execution begins.
 */
void Console::quest_running() {

}

/**
 * @brief Slot called when the quest execution is finished.
 */
void Console::quest_finished() {

}

/**
 * @brief Slot called when the quest execution produced some output lines.
 * @param lines The lines read from the standard output of the quest.
 */
void Console::quest_output_produced(const QStringList& lines) {

  for (const QString& line : lines) {
    parse_output(line);
  }
}

/**
 * @brief Slot called when the user wants to execute a Lua instruction from the console.
 */
void Console::command_field_activated() {

  if (quest_runner == nullptr) {
    return;
  }

  if (!quest_runner->is_running()) {
    return;
  }

  const QString& command = ui.command_field->text();

  execute_command(command);
  ui.command_field->command_executed(command);
}

/**
 * @brief Executes some Lua code in the quest process and logs it into the console.
 * @param command The Lua code.
 * @return @c true if the code could be sent to the process
 * (even if it produces an error).
 */
bool Console::execute_command(const QString& command) {

  if (quest_runner == nullptr) {
    return false;
  }

  if (!quest_runner->is_started()) {
    return false;
  }

  quest_runner->execute_command(command);

  // Show the command in the log view.
  // TODO: show it only when receiving its results, to make sure it is displayed
  // just before its results.
  ui.log_view->appendPlainText("> " + command);
  return true;
}

/**
 * @brief Parses a Solarus output line and handles it.
 * @param line The output line.
 */
void Console::parse_output(const QString& line) {

  if (line.isEmpty()) {
    return;
  }

  QRegularExpressionMatch match_result = output_regexp.match(line);
  if (!match_result.hasMatch()) {
    // Not a line from Solarus, probably one from the quest.
    ui.log_view->appendPlainText(line);
    return;
  }

  // 4 captures expected: full line, time, log level, message.
  QStringList captures = match_result.capturedTexts();
  if (captures.size() != 4) {
    ui.log_view->appendPlainText(line);
    return;
  }

  QString log_level = captures[2];
  QString message = captures[3];

  // Filter out technical delimiters of commands output.
  if (message.startsWith("====== Begin Lua command #")
      || message.startsWith("====== End Lua command #")) {
    return;
  }

  // Report system setting changes.
  detect_setting_change(log_level, message);

  if (log_level == "Error") {
    // Clean specific error messages.
    message.remove(output_simplify_console_error_regexp);
  }

  // Add color.
  QString line_html = colorize_output(log_level, message);
  if (line_html.isEmpty()) {
    return;
  }

  ui.log_view->appendHtml(line_html);
}

/**
 * @brief Emits the signal system_setting_changed if an output message
 * indicates that a setting has just changed.
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 */
void Console::detect_setting_change(
    const QString& log_level,
    const QString& message) {

  if (log_level != "Info") {
    return;
  }

  QRegularExpressionMatch match_result;

  match_result = output_setting_video_mode_regexp.match(message);
  if (match_result.lastCapturedIndex() == 1) {
    QVariant value = match_result.captured(1);
    emit setting_changed_in_quest("quest_video_mode", value);
    return;
  }

  match_result = output_setting_fullscreen_regexp.match(message);
  if (match_result.lastCapturedIndex() == 1) {
    QVariant value = (match_result.captured(1) == "yes");
    emit setting_changed_in_quest("quest_fullscreen", value);
    return;
  }
}

/**
 * @brief Returns a colorized version of a Solarus output line.
 *
 * Colors may be added.
 *
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 * @return The HTML decorated line.
 */
QString Console::colorize_output(const QString& log_level, const QString& message) {

  if (message.isEmpty()) {
    return message;
  }

  QString decorated_line = log_level + ": " + message;

  // Colorize warnings and errors.
  if (log_level == "Debug") {
    decorated_line = colorize(decorated_line, "#808080");
  }
  else if (log_level == "Info") {
    decorated_line = colorize(decorated_line, "#0000ff");
  }
  else if (log_level == "Warning") {
    decorated_line = colorize(decorated_line, "#b05000");
  }
  else if (log_level == "Error") {
    decorated_line = colorize(decorated_line, "#ff0000");
  }
  else if (log_level == "Fatal") {
    decorated_line = colorize(decorated_line, "#ff0000");
  }
  else {
    // Unknown log level.
    decorated_line = message;
  }

  return decorated_line;
}

}
