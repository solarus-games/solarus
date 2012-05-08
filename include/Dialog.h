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
#ifndef SOLARUS_DIALOG_H
#define SOLARUS_DIALOG_H

#include "Common.h"
#include <list>

/**
 * @brief A dialog that can be displayed during the game in the dialog box.
 */
class Dialog {

  public:

    /**
     * Indicates what happens when the user tries to skip a dialog.
     */
    enum SkipMode {
      SKIP_NONE,      /**< the current dialog cannot be skipped */
      SKIP_CURRENT,   /**< the player can display the 3 current lines immediately */
      SKIP_ALL,       /**< the dialog box can be totally skipped */
      SKIP_UNCHANGED  /**< keep the skip property of the previous dialog */
    };

    Dialog();
    Dialog(const Dialog& other);
    ~Dialog();
    Dialog& operator=(const Dialog& other);

    int get_icon() const;
    void set_icon(int icon);
    SkipMode get_skip_mode() const;
    void set_skip_mode(SkipMode skip_mode);
    bool is_question() const;
    void set_question(bool question);
    bool has_next() const;
    const std::string& get_next() const;
    void set_next(const std::string& next);
    const std::string& get_next2() const;
    void set_next2(const std::string& next2);
    const std::string& get_text() const;
    const std::list<std::string>& get_lines() const;
    void set_text(const std::string& text);

  private:

    int icon;                     /**< index of an icon to show (-1 for none, -2 for unchanged) */
    SkipMode skip_mode;           /**< skip mode of this dialog */
    bool question;                /**< is this dialog a question? */
    std::string next;             /**< id of the next dialog if any (possibly "_unknown") */
    std::string next2;            /**< id of the alternative next dialog in a question */
    std::string text;             /**< the whole text of this dialog */
    std::list<std::string> lines; /**< the text split in lines */
};

#endif

