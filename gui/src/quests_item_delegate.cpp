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
#include "solarus/gui/quests_item_delegate.h"
#include "solarus/gui/quests_model.h"

#include <QPainter>
#include <QRect>
#include <QFont>
#include <QFontMetrics>
#include <QStringList>
#include <QSize>
#include <QIcon>

namespace SolarusGui {

QuestsItemDelegate::QuestsItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent), _iconSize(32, 32) {}

QuestsItemDelegate::~QuestsItemDelegate() {}

void QuestsItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const {

  // Paint the default background
  //QStyledItemDelegate::paint(painter, option, index);

  // Save painter initial state before any modification
  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setRenderHint(QPainter::TextAntialiasing, true);

  const auto &global_rect = option.rect;
  const auto padding = 4;
  const auto &default_font = option.font;

  // Retrieve quest info
  auto data = index.model()->data(index, Qt::DisplayRole);
  auto quest_info =data.value<QuestsModel::QuestInfo>();

  // Retrieve info from option
  const auto &state = option.state;
  const auto &features = option.features;
  const auto &palette = option.palette;
  auto active = state.testFlag(QStyle::State_Active);
  auto enabled = state.testFlag(QStyle::State_Enabled);
  auto selected = state.testFlag(QStyle::State_Selected);
  auto alternate_rows = features.testFlag(QStyleOptionViewItem::Alternate);
  auto should_alternate = alternate_rows && index.row() % 2;
  //auto focused = state.testFlag(QStyle::State_HasFocus);
  //auto mouse_over = state.testFlag(QStyle::StateFlag::State_MouseOver);

  // Compute colors
  auto color_group =
      !enabled ? QPalette::Disabled
               : (active ? QPalette::Active : QPalette::Inactive);
  auto color_role =
      selected ? QPalette::Highlight
               : (should_alternate ? QPalette::AlternateBase : QPalette::Light);
  auto background = palette.brush(color_group, color_role);

  // Paint background
  painter->fillRect(option.rect, background);

  // Paint icon
  QRect icon_rect(
      global_rect.left() + padding * 2,
      global_rect.top() + (global_rect.height() - _iconSize.height()) / 2,
      _iconSize.width(),
      _iconSize.height());
  auto icon_mode = enabled ? QIcon::Normal : QIcon::Disabled;
  auto pixmap = quest_info.icon.pixmap(_iconSize,
                                       icon_mode, QIcon::On);
  painter->drawPixmap(icon_rect, pixmap);

  // Compute title font
  auto title_font = default_font;
  title_font.setPointSize(title_font.pointSize() * 1.25);
  title_font.setWeight(QFont::DemiBold);

  // Compute author font
  auto author_font = default_font;
  author_font.setWeight(QFont::Normal);

  // Set temporarly the font to compute the title text height
  painter->save();
  painter->setFont(title_font);
  auto title_font_metrics = painter->fontMetrics();
  painter->restore();

  // Set temporarly the font to compute the author text height
  painter->save();
  painter->setFont(author_font);
  auto author_font_metrics = painter->fontMetrics();
  painter->restore();

  // Compute author and title text rects
  auto title_rect_left = icon_rect.right() + padding * 3;
  auto title_rect_width = global_rect.width() - title_rect_left - padding;
  auto title_rect_height = title_font_metrics.height();

  auto author_rect_left = title_rect_left;
  auto author_rect_width = title_rect_width;
  auto author_rect_height = author_font_metrics.height();

  QRect title_rect(title_rect_left, 0, title_rect_width, title_rect_height);
  QRect author_rect(author_rect_left, title_rect_height, author_rect_width,
                    author_rect_height);

  auto global_text_height = author_rect.bottom() - title_rect.top();
  auto global_text_top = (global_rect.height() - global_text_height) / 2;

  auto y_translate = global_rect.top() + global_text_top;
  title_rect.translate(0, y_translate);
  author_rect.translate(0, y_translate);

  // Compute title text color
  auto pen_color_role = selected ? QPalette::HighlightedText
                               : QPalette::Text;
  auto title_pen_color = palette.brush(color_group, pen_color_role).color();

  // Paint title text
  auto title_text = QString::fromStdString(quest_info.properties.get_title());
  if(!title_text.isEmpty()){
    auto title_elided_text = title_font_metrics.elidedText(title_text,
                                                           Qt::ElideRight,
                                                           title_rect_width);
    painter->setPen(title_pen_color);
    painter->setFont(title_font);
    painter->drawText(title_rect, title_elided_text);
  }

  // Compute title text color
  auto author_pen_color =
      palette.brush(QPalette::Disabled, pen_color_role).color();

  // Paint author text
  auto separator = QString(" %1 ").arg(QChar(0x2022)); // bullet

  QStringList secondarInfo;
  auto author_text =
          QString::fromStdString(quest_info.properties.get_author());
  auto date_text =
          QString::fromStdString(quest_info.properties.get_release_date());
  if(!date_text.isEmpty()){
    secondarInfo << date_text;
  }
  if(!author_text.isEmpty()){
    secondarInfo << author_text;
  }

  if(!secondarInfo.isEmpty()){
    auto secondary_text = secondarInfo.join(separator);
    auto secondary_elided_text = author_font_metrics.elidedText(
        secondary_text, Qt::ElideRight, author_rect_width);
    painter->setPen(author_pen_color);
    painter->setFont(author_font);
    painter->drawText(author_rect, secondary_elided_text);
  }
  // Restore painter initial state
  painter->restore();
}

QSize QuestsItemDelegate::sizeHint(const QStyleOptionViewItem &,
                             const QModelIndex &) const {
  return QSize(150, _iconSize.height() + 16);
}

const QSize &QuestsItemDelegate::iconSize() const {
  return _iconSize;
}

void QuestsItemDelegate::setIconSize(const QSize &iconSize) {
  _iconSize = iconSize;
}

} // namespace SolarusGui
