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
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/CurrentQuest.h"
#include "solarus/QuestProperties.h"
#include <QApplication>
#include <algorithm>

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

  switch (role) {
  case Qt::ItemDataRole::DisplayRole:
    return QVariant::fromValue(quests.at(index.row()));
  case Qt::ItemDataRole::ToolTipRole:
    return QString::fromStdString(
            quests.at(index.row()).properties.get_title());
  default:
    return QVariant();
  }
}

/**
 * @brief Finds the index of a quest in the list.
 * @param quest_path Path of the quest to look for.
 * @return The quest index, or -1 if it is not in the list.
 */
int QuestsModel::path_to_index(const QString& quest_path) const {

  for (size_t i = 0; i < quests.size(); ++i) {
    if (quests[i].path == quest_path) {
      return i;
    }
  }

  return -1;
}

/**
 * @brief Returns the quest path at the given index.
 * @param quest_index Index of the quest to get.
 * @return The path, or an empty string if the index is invalid.
 */
QString QuestsModel::index_to_path(int quest_index) const {

  if (quest_index < 0 || quest_index >= (int) quests.size()) {
    return QString();
  }

  return quests[quest_index].path;
}

/**
 * @brief Returns whether the model contains the given quest path.
 * @param quest_path Quest path to test.
 * @return @c true if it is in the model.
 */
bool QuestsModel::has_quest(const QString& quest_path) {

  return path_to_index(quest_path) != -1;
}

/**
 * @brief Adds a quest to the model.
 * @param quest_path Path of the quest to add.
 * @return @c true if it was added, @c false if there is no such quest
 * or if it is already there.
 */
bool QuestsModel::add_quest(const QString& quest_path) {

  if (has_quest(quest_path)) {
    return false;
  }

  QuestInfo info;

  // Open the quest to get its quest.dat file.
  QStringList arguments = QApplication::arguments();
  QString program_name = arguments.isEmpty() ? QString() : arguments.first();
  if (!Solarus::QuestFiles::open_quest(program_name.toStdString(),
                                       quest_path.toStdString())) {
    Solarus::QuestFiles::close_quest();
    return false;
  }
  info.properties = Solarus::CurrentQuest::get_properties();
  Solarus::QuestFiles::close_quest();

  const int num_quests = rowCount();
  beginInsertRows(QModelIndex(), num_quests, num_quests);

  info.path = quest_path;
  info.directory_name = quest_path.section('/', -1, -1, QString::SectionSkipEmpty);
  info.icon = get_quest_default_icon();  // TODO
  info.logo = get_quest_default_logo(); // TODO
  quests.push_back(info);

  endInsertRows();

  return true;
}

/**
 * @brief Removes a quest from this model.
 * @param quest_index Index of the quest to remove.
 * @return @c true if the quest was removed, @c false if there is no quest
 * with this index.
 */
bool QuestsModel::remove_quest(int quest_index) {

  if (quest_index < 0 || quest_index > rowCount()) {
    return false;
  }

  beginRemoveRows(QModelIndex(), quest_index, quest_index);
  quests.erase(quests.begin() + quest_index);
  endRemoveRows();
  return true;
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

/**
 * @brief Returns the properties of a quest in the model.
 * @param quest_index Index of the quest to get.
 * @return The quest properties.
 */
Solarus::QuestProperties QuestsModel::get_quest_properties(int quest_index) const {

  if (quest_index < 0 || quest_index > rowCount()) {
    return Solarus::QuestProperties();
  }

  return quests[quest_index].properties;
}

/**
 * @brief Returns the default logo for a quest.
 * @return The default logo
 */
const QPixmap& QuestsModel::get_quest_default_logo() const {

  static const QPixmap default_logo(":/images/no_logo.png");

  return default_logo;
}

/**
 * @brief Returns the quest logo.
 * @param quest_index Index of the quest to get.
 * @return The quest logo.
 */
const QPixmap& QuestsModel::get_quest_logo(int quest_index) const {

  if (quest_index < 0 || quest_index > rowCount()) {
    return get_quest_default_logo();
  }

  return quests[quest_index].logo;
}

/**
 * @brief Returns the default icon for a quest.
 * @return The default icon.
 */
const QIcon &QuestsModel::get_quest_default_icon() const {

    static const QIcon default_icon(":/images/default_icon.png");

    return default_icon;
}

/**
 * @brief Sort the quests and notify the view (more convenient than sort(int, order).
 * @param sortType Way to sort the quests in the list by
 * @param order Order to the quests in the list by
 */
void QuestsModel::sort(QuestSort sortType, Qt::SortOrder order) {

  sort((int)sortType, order);
}

/**
 * @brief Sort the quests and notify the view
 * @param column int value of a QuestSort value, column to sort the quests by
 * @param order Order to the quests in the list by
 */
void QuestsModel::sort(int column, Qt::SortOrder order) {

  doSort((QuestSort)column, order);
}

/**
 * @brief Do the actual sort, without notifying the view
 * @param sortType Way to sort the quests in the list by
 * @param order Order to sort the quests in the list by
 */
void QuestsModel::doSort(QuestSort sortType, Qt::SortOrder order) {

  std::sort(quests.begin(), quests.end(),
    [sortType, order](const QuestInfo &a, const QuestInfo &b) {
      auto ascending = order == Qt::AscendingOrder;
      switch (sortType) {
      case SortByAuthor:
        return ascending ? a.properties.get_author() < a.properties.get_author()
                         : a.properties.get_author() > a.properties.get_author();
      case SortByDate:
        return ascending ? a.properties.get_release_date() < b.properties.get_release_date()
                         : a.properties.get_release_date() > b.properties.get_release_date();
      case SortByName:
      default:
        return ascending ? a.properties.get_title() < b.properties.get_title()
                         : a.properties.get_title() > b.properties.get_title();
      }
    });
}

} // namespace SolarusGui
