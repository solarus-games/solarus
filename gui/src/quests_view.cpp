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
#include "solarus/gui/quests_item_delegate.h"

namespace SolarusGui {

/**
 * @brief Creates a quests view.
 * @param parent Parent object or nullptr.
 */
QuestsView::QuestsView(QWidget* parent) :
  QListView(parent),
  model(nullptr), itemDelegate(nullptr) {

  model = new QuestsModel(this);
  itemDelegate = new QuestsItemDelegate(this);
  setItemDelegate(itemDelegate);
  setModel(model);
}

/**
 * @brief Finds the index of a quest in the list.
 * @param path Path of the quest to look for.
 * @return The quest index, or -1 if it is not in the list.
 */
int QuestsView::path_to_index(const QString& path) const {

  return model->path_to_index(path);
}

/**
 * @brief Returns the quest path at the given index.
 * @param index Index of the quest to get.
 * @return The path, or an empty string if the index is invalid.
 */
QString QuestsView::index_to_path(int index) const {

  return model->index_to_path(index);
}

/**
 * @brief Returns the number of quests in this view.
 * @return The number of quests.
 */
int QuestsView::get_num_quests() const {

  return model->rowCount();
}

/**
 * @brief Returns the list of quests paths in the view.
 * @return The quests paths.
 */
QStringList QuestsView::get_paths() const {

  return model->get_paths();
}

/**
 * @brief Returns whether the view contains the given quest path.
 * @param quest_path Quest path to test.
 * @return @c true if it is in the view.
 */
bool QuestsView::has_quest(const QString& path) {

  return model->has_quest(path);
}

/**
 * @brief Adds a quest to the model of this view.
 * @param quest_path Path of the quest to add.
 * @return @c true if it was added, @c false if it was already there.
 */
bool QuestsView::add_quest(const QString& path) {

  return model->add_quest(path);
}

/**
 * @brief Removes a quest from the model of this view.
 * @param index Index of the quest to remove.
 * @return @c true if the quest was removed, @c false if there is no quest
 * with this index.
 */
bool QuestsView::remove_quest(int index) {

  return model->remove_quest(index);
}

/**
 * @brief Returns the properties of the selected quest.
 * @return The quest properties, or a default-constructed object
 * if no quest is selected.
 */
Solarus::QuestProperties QuestsView::get_selected_quest_properties() const {

  int index = get_selected_index();
  if (index == -1) {
    return Solarus::QuestProperties();
  }
  return get_quest_properties(index);
}

/**
 * @brief Returns the properties of a quest in this view.
 * @param index Index of the quest to get.
 * @return The quest properties.
 */
Solarus::QuestProperties QuestsView::get_quest_properties(int index) const {

  return model->get_quest_properties(index);
}

/**
 * @brief Returns the index of the currently selected quest.
 * @return Return current index, or -1 if no quest is selected.
 */
int QuestsView::get_selected_index() const {

  QModelIndexList model_indexes = selectionModel()->selectedIndexes();
  if (model_indexes.isEmpty()) {
    return -1;
  }
  return model_indexes.first().row();
}

/**
 * @brief Returns the quest path currently selected.
 * @return The current quest path, or an empty string if no quest is selected.
 */
QString QuestsView::get_selected_path() const {

  return index_to_path(get_selected_index());
}

/**
 * @brief Selects a quest.
 * @param index Index of the quest to make selected.
 */
void QuestsView::select_quest(int index) {

  if (index < 0 || index >= get_num_quests()) {
    return;
  }

  selectionModel()->select(model->index(index, 0), QItemSelectionModel::ClearAndSelect);
}

/**
 * @brief Selects the given quest if it is in the view.
 * @param path Path of the quest to make selected.
 */
void QuestsView::select_quest(const QString& path) {

  int index = path_to_index(path);
  if (index == -1) {
    return;
  }
  select_quest(index);
}

}
