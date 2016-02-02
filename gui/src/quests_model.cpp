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

namespace SolarusGui {

/**
 * @brief Creates a quests view.
 * @param parent Parent object or nullptr.
 */
QuestsModel::QuestsModel(QObject* parent) :
  QAbstractListModel(parent),
  quests() {

}

/**
 * @brief Returns the number of quests.
 * @param parent Parent index.
 * @return The number of quests.
 */
int QuestsModel::rowCount(const QModelIndex& parent) const {

  Q_UNUSED(parent);
  return quests.size();
}

/**
 * @brief Returns the data of an item for the given role.
 * @param index Index of the item to get.
 * @param role Kind of data to get.
 * @return The data for this item and this role.
 */
QVariant QuestsModel::data(const QModelIndex& index, int role) const {

  if (index.row() < 0 || index.row() >= rowCount()) {
    return QVariant();
  }

  const QuestInfo& info = quests[index.row()];

  switch (role) {

  case Qt::DisplayRole:
    return info.quest_title;

  case Qt::DecorationRole:
    return info.icon;

  }

  return QVariant();
}

/**
 * @brief Returns whether the model contains the given quest path.
 * @param quest_path Quest path to test.
 * @return @c true if it is in the model.
 */
bool QuestsModel::has_quest(const QString& quest_path) {

  for (const QuestInfo& info : quests) {
    if (info.path == quest_path) {
      return true;
    }
  }

  return false;
}

/**
 * @brief Adds a quest to the model.
 * @param quest_path Path of the quest to add.
 * @return @c true if it was added, @c false if it was already there.
 */
bool QuestsModel::add_quest(const QString& quest_path) {

  if (has_quest(quest_path)) {
    return false;
  }

  const int num_quests = rowCount();
  beginInsertRows(QModelIndex(), num_quests, num_quests);

  QuestInfo info;
  info.path = quest_path;
  info.directory_name = quest_path.section('/', -1, -1, QString::SectionSkipEmpty);
  info.quest_title = info.directory_name;  // TODO
  info.icon = QIcon(":/images/logo_solarus_200.png");  // TODO

  quests.push_back(info);

  endInsertRows();

  // TODO save settings
}

/**
 * @brief Returns the list of quests paths in the model.
 * @return The quests paths.
 */
QStringList QuestsModel::get_paths() const {

  QStringList paths;
  for (const QuestInfo& info : quests) {
    paths << info.path;
  }
  return paths;
}

}
