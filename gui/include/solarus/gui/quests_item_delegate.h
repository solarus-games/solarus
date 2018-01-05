/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_GUI_QUEST_RUNNER_H
#define SOLARUS_GUI_QUEST_RUNNER_H

#include "solarus/gui/gui_common.h"
#include <QStyledItemDelegate>

#include <QSize>
#include <QPalette>

namespace SolarusGui {

/**
 * @brief ItemDelegate to draw the quests info
 */
class SOLARUS_GUI_API QuestsItemDelegate : public QStyledItemDelegate {

public:
  QuestsItemDelegate(QObject *parent = nullptr);
  ~QuestsItemDelegate();

public:
  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;

public:
  const QSize &iconSize() const;
  void setIconSize(const QSize &iconSize);

private:
  QSize _iconSize;
};

} // namespace SolarusGui

#endif
