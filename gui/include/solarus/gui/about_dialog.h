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
#ifndef SOLARUS_GUI_ABOUT_DIALOG_H
#define SOLARUS_GUI_ABOUT_DIALOG_H

#include "solarus/gui/gui_common.h"
#include <QDialog>
#include "ui_about_dialog.h"

namespace SolarusGui {

/**
 * @brief About Dialog of the Solarus GUI.
 */
class SOLARUS_GUI_API AboutDialog : public QDialog {
  Q_OBJECT

public:

  explicit AboutDialog(QWidget* parent = nullptr);

private:

  Ui::AboutDialog ui;         /**< The widgets. */
};

} // SolarusGui

#endif
