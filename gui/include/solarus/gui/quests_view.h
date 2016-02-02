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
#ifndef SOLARUS_GUI_QUESTS_VIEW_H
#define SOLARUS_GUI_QUESTS_VIEW_H

#include "solarus/Common.h"
#include <QListView>

namespace SolarusGui {

class QuestsModel;

/**
 * @brief A widget where the user can select a quest.
 */
class QuestsView : public QListView {

public:

  QuestsView(QWidget* parent = nullptr);

  bool add_quest(const QString& quest_path);
  QStringList get_paths() const;

private:

  QuestsModel* model;

};

}

#endif
