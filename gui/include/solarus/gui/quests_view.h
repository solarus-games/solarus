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

#include "solarus/gui/gui_common.h"
#include "solarus/QuestProperties.h"
#include <QListView>

namespace SolarusGui {

class QuestsModel;
class QuestsItemDelegate;

/**
 * @brief A widget where the user can select a quest.
 */
class SOLARUS_GUI_API QuestsView : public QListView {

public:

  QuestsView(QWidget* parent = nullptr);

  int path_to_index(const QString& path) const;
  QString index_to_path(int index) const;

  int get_num_quests() const;
  QStringList get_paths() const;
  bool has_quest(const QString& path);
  bool add_quest(const QString& path);
  bool remove_quest(int index);

  Solarus::QuestProperties get_selected_quest_properties() const;
  Solarus::QuestProperties get_quest_properties(int index) const;

  const QPixmap& get_selected_logo() const;
  const QPixmap& get_quest_logo(int index) const;

  int get_selected_index() const;
  QString get_selected_path() const;
  void select_quest(int index);
  void select_quest(const QString& path);

private:

  QuestsModel* model;
  QuestsItemDelegate* itemDelegate;
};

}

#endif
