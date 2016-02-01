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
#include "solarus/gui/quests_model.h"
#include "solarus/gui/quests_view.h"

namespace SolarusGui {

/**
 * @brief Creates a quests view.
 * @param parent Parent object or nullptr.
 */
QuestsView::QuestsView(QWidget* parent) :
  QListView(parent),
  model(nullptr) {

  setViewMode(ViewMode::IconMode);
  setMovement(Movement::Static);
  setGridSize(QSize(200, 200));
  setIconSize(QSize(128, 128));

  model = new QuestsModel(this);
  setModel(model);
}

/**
 * @brief Adds a quest to the model of this view.
 * @param quest_path Path of the quest to add.
 */
void QuestsView::add_quest(const QString& quest_path) {

  model->add_quest(quest_path);
}

}
