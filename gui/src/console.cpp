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
#include "solarus/gui/settings.h"
#include <QDebug>
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

const QRegularExpression output_regexp("^\\[Solarus\\] \\[(\\d+)\\] (\\w*): (.+)$");
const QRegularExpression output_command_result_begin_regexp("^====== Begin Lua command #(\\d+) ======$");
const QRegularExpression output_command_result_end_regexp("^====== End Lua command #(\\d+): (\\w+) ======$");
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
  quest_runner(),
  pending_commands(),
  output_command_id(-1),
  command_enabled(true) {

  ui.setupUi(this);
}

/**
 * @brief Returns whether the Lua command input field is available.
 * @return @c true if the user can type Lua commands.
 */
bool Console::is_command_enabled() const {
  return command_enabled;
}

/**
 * @brief Sets whether the Lua command input field is available.
 * @param enable @c true to allow user to type Lua commands.
 */
void Console::set_command_enabled(bool enable) {

  this->command_enabled = enable;
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

  // Apply settings to the running quest as Lua commands,
  // for quests that don't read the settings.dat file.
  apply_settings();
}

/**
 * @brief Slot called when the quest execution is finished.
 */
void Console::quest_finished() {

  output_command_id = -1;
  pending_commands.clear();
  output_command_result.clear();
}

/**
 * @brief Slot called when the quest execution produced some output lines.
 * @param lines The lines read from the standard output of the quest.
 */
void Console::quest_output_produced(const QStringList& lines) {

  Q_FOREACH (const QString& line, lines) {
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
 *
 * Use the command_result_received() signal to be notified of the result if you
 * need to.
 *
 * @param command The Lua code.
 * @return The id of the command executed, or -1 if it could not be sent
 * to the process.
 */
int Console::execute_command(const QString& command) {

  if (quest_runner == nullptr) {
    return -1;
  }

  if (!quest_runner->is_started()) {
    return -1;
  }

  if (command.isEmpty()) {
    return -1;
  }

  int command_id = quest_runner->execute_command(command);
  pending_commands[command_id] = command;

  return command_id;
}

/**
 * @brief Parses a Solarus output line and handles it.
 * @param line The output line.
 */
void Console::parse_output(const QString& line) {

  if (line.isEmpty()) {
    return;
  }

  QString log_level;
  QString message = line;
  QRegularExpressionMatch match_result = output_regexp.match(line);

  if (match_result.hasMatch()) {
    // 4 captures expected: full line, time, log level, message.
    QStringList captures = match_result.capturedTexts();
    if (captures.size() != 4) {
      ui.log_view->appendHtml(line.toHtmlEscaped());
      return;
    }

    log_level = captures[2];
    message = captures[3];
  }

  if (!log_level.isEmpty() && message.isEmpty()) {
    // Solarus produced an empty message just to flush its stdout.
    return;
  }

  // Detect technical delimiters of commands output but don't show them.
  if (detect_command_result(log_level, message)) {
    return;
  }

  if (log_level.isEmpty()) {
    // Not a line from Solarus, probably one from the quest.
    ui.log_view->appendHtml(line.toHtmlEscaped());
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
 * @brief Detects output messages that are the result of a command that was
 * send from the console.
 * @param log_level The Solarus log level of the line.
 * @param message The rest of the message.
 * @return @c true if the message is a command result delimiter and was consumed.
 */
bool Console::detect_command_result(
    const QString& log_level,
    const QString& message) {

  QRegularExpressionMatch match_result;

  // Detect the beginning of a console command result.
  match_result = output_command_result_begin_regexp.match(message);
  if (log_level == "Info" && match_result.lastCapturedIndex() == 1) {
    // Start of a command result.
    if (output_command_id != -1) {
      qWarning() << "Beginning of a command result inside another command result";
    }

    output_command_id = match_result.captured(1).toInt();
    output_command_result = QString();

    // Show the command in the log view.
    // We show the command only when receiving its results,
    // to make sure it is displayed just before its results.
    QString command = pending_commands.take(output_command_id);
    ui.log_view->appendHtml(QString("> %1").arg(command).toHtmlEscaped());

    return true;
  }

  // Detect the end of a console command result.
  match_result = output_command_result_end_regexp.match(message);
  if (log_level == "Info" && match_result.lastCapturedIndex() == 2) {
    // End of a command result.
    if (output_command_id == -1) {
      qWarning() << "End of a command result without beginning";
      return false;
    }
    int id = match_result.captured(1).toInt();
    bool success = (match_result.captured(2) == "success");
    QString command = pending_commands.take(output_command_id);
    QString result = output_command_result;

    if (id != output_command_id) {
      qWarning() << "Unmatched command delimiters";
    }

    pending_commands.remove(output_command_id);
    output_command_id = -1;
    output_command_result.clear();

    emit command_result_received(id, command, success, result);

    return true;
  }

  // Process the current result between delimiters.
  if (output_command_id != -1) {
    // We are inside the delimiters.
    output_command_result += message;
    return false;  // Let the console colorize and show the text normally.
  }

  return false;
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

/**
 * @brief Returns Lua commands that set the given settings in the Solarus API.
 *
 * System settings like audio volume and the video mode are returned as
 * Lua commands.
 * This function allows to apply to a running quest process
 * what the user chose in the GUI.
 */
QStringList Console::get_quest_lua_commands_from_settings() const {

  Settings settings;
  QStringList commands;
  QVariant video_mode = settings.value("quest_video_mode");
  if (video_mode.isValid()) {
    commands << QString("sol.video.set_mode(\"%1\")").
                arg(video_mode.toString());
  }

  QVariant fullscreen = settings.value("quest_fullscreen");
  if (fullscreen.isValid()) {
    commands << QString("sol.video.set_fullscreen(%1)").
                arg(fullscreen.toBool() ? "true" : "false");
  }

  QVariant sound_volume = settings.value("quest_sound_volume");
  if (sound_volume.isValid()) {
    commands << QString("sol.audio.set_sound_volume(%1)").
                arg(sound_volume.toInt());
  }

  QVariant music_volume = settings.value("quest_music_volume");
  if (music_volume.isValid()) {
    commands << QString("sol.audio.set_music_volume(%1)").
                arg(music_volume.toInt());
  }

  QVariant language = settings.value("quest_language");
  if (language.isValid()) {
    commands << QString("sol.language.set_language(\"%1\")").
                arg(language.toString());
  }

  return commands;
}

/**
 * @brief Applies current settings of the GUI to the quest process.
 *
 * Settings are applied as Lua commands.
 *
 * @param settings The settings.
 * @return @c true if settings commands could be sent to the process
 * (even if they produce an error).
 */
bool Console::apply_settings() {

  if (quest_runner == nullptr || !quest_runner->is_running()) {
    return false;
  }

  QStringList commands = get_quest_lua_commands_from_settings();
  bool success = true;
  Q_FOREACH (const QString& command, commands) {
    success = execute_command(command) && success;
  }
  return success;
}

}
