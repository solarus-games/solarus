/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "Dialog.h"

/**
 * @brief Constructor.
 */
Dialog::Dialog():
  icon(-2),
  skip_mode(SKIP_UNCHANGED),
  question(false),
  next(""),
  next2(""),
  text("") {

}

/**
 * @brief Copy constructor.
 * @param other the dialog to copy
 */
Dialog::Dialog(const Dialog& other):
  icon(other.icon),
  skip_mode(other.skip_mode),
  question(other.question),
  next(other.next),
  next2(other.next2),
  text(other.text),
  lines(other.lines) {

}

/**
 * @brief Destructor
 */
Dialog::~Dialog() {

}

/**
 * @brief Assignment operator.
 * @param other the dialog to copy
 * @return this dialog
 */
Dialog& Dialog::operator=(const Dialog& other) {

  if (&other != this) {
    this->icon = other.icon;
    this->skip_mode = other.skip_mode;
    this->question = other.question;
    this->next = other.next;
    this->next2 = other.next2;
    this->text = other.text;
    this->lines = other.lines;
  }

  return *this;
}

/**
 * @brief Returns the index of icon of this dialog.
 * @return the icon index, or -1 for no icon
 */
int Dialog::get_icon() const {

  return icon;
}

/**
 * @brief Sets the icon of this dialog.
 * @param icon the icon index to set, or -1 for no icon
 */
void Dialog::set_icon(int icon) {

  this->icon = icon;
}

/**
 * @brief Returns the skip mode of this dialog.
 * @return the skip mode
 */
Dialog::SkipMode Dialog::get_skip_mode() const {

  return skip_mode;
}

/**
 * @brief Sets the skip mode of this dialog.
 * @param skip_mode the skip mode
 */
void Dialog::set_skip_mode(SkipMode skip_mode) {

  this->skip_mode = skip_mode;
}

/**
 * @brief Returns whether this dialog ends with a question.
 * @return true if the last three lines are a question and two answers
 */
bool Dialog::is_question() const {

  return question;
}

/**
 * @brief Sets whether this dialogs ends with a question.
 * @param question true to make the last three lines be a question and two
 * answers
 */
void Dialog::set_question(bool question) {

  this->question = question;
}

/**
 * @brief Returns whether this dialog if followed by another one.
 * @return true if there is a next dialog (even if it is not known yet)
 */
bool Dialog::has_next() const {

  return next.size() > 0;
}

/**
 * @brief Returns the id of the dialog that follows this one.
 * @return the id of the next dialog, an empty string if there is no next
 * dialog, or "_unknown" if the next dialog exists but is not known yet
 */
const std::string& Dialog::get_next() const {

  return next;
}

/**
 * @brief Sets the id of the dialog that follows this one.
 * @param next the id of the next dialog, an empty string if there is no next
 * dialog, or "_unknown" if the next dialog exists but is not known yet
 */
void Dialog::set_next(const std::string& next) {

  this->next = next;
}

/**
 * @brief Returns the id of the alternative next dialog after a question.
 * @return the id of the next dialog if the player chooses the second answer
 * if this dialog is a question, or an empty string if there is no question
 */
const std::string& Dialog::get_next2() const {

  return next2;
}

/**
 * @brief Sets the id of the alternative next dialog after a question.
 * @param next2 id of the next dialog if the player chooses the second answer
 * if this dialog is a question
 */
void Dialog::set_next2(const std::string& next2) {

  this->next2 = next2;
}

/**
 * @brief Returns the whole text of this dialog.
 *
 * Lines are separated by '\n'.
 *
 * @return the text of this dialog
 */
const std::string& Dialog::get_text() const {

  return text;
}

/**
 * @brief Returns the text of this dialog split in lines.
 * @return the lines of text
 */
const std::list<std::string>& Dialog::get_lines() const {

  return lines;
}

/**
 * @brief Sets the text of this dialog.
 *
 * Lines must be separated by '\n'. The last line of text may end with '\n'.
 *
 * @param text the text of this dialog
 */
void Dialog::set_text(const std::string& text) {

  this->text = text;
  size_t start = 0;
  size_t end;
  do {
    end = text.find_first_of('\n', start);
    const std::string line = text.substr(start, end - start);
    if (end != std::string::npos) {
      lines.push_back(line);
    }
    start = end + 1;
  }
  while (end != std::string::npos);
}

