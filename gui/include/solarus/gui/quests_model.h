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
#ifndef SOLARUS_GUI_QUESTS_MODEL_H
#define SOLARUS_GUI_QUESTS_MODEL_H

#include "solarus/Common.h"
#include <QAbstractListModel>
#include <QIcon>

namespace SolarusGui {

/**
 * @brief List of quests added to Solarus.
 */
class QuestsModel : public QAbstractListModel {

public:

  QuestsModel(QObject* parent = nullptr);

  // QAbstractListModel API
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  bool has_quest(const QString& quest_path);
  bool add_quest(const QString& quest_path);
  QStringList get_paths() const;

private:

  /**
   * @brief Info of a quest from the list.
   */
  struct QuestInfo {
    QString path;                   /**< Path to the quest directory. */
    QString directory_name;         /**< Name of the quest directory. */
    QString quest_title;            /**< Title of the quest. */
    QIcon icon;                     /**< Icon of the quest. */
  };

  std::vector<QuestInfo> quests;    /**< Info of each quest in the list. */
};

}

#endif
