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
#include "solarus/gui/settings.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/Settings.h"
#include <QApplication>

namespace SolarusGui {

/**
 * @brief Creates a Solarus settings object.
 */
Settings::Settings() :
  QSettings(QSettings::SystemScope, "solarus", "solarus") {

}

/**
 * @brief Export GUI settings to the Solarus settings.dat file of a quest.
 *
 * System settings like audio volume and the video mode are written to
 * settings.dat.
 * It allows quests that use settings.dat to apply what the user chose
 * in the GUI.
 *
 * @param quest_path Path of the quest where to write a settings file.
 */
void Settings::export_to_quest(const QString& quest_path) const {

  if (Solarus::QuestFiles::is_open()) {
    Solarus::QuestFiles::close_quest();
  }

  QStringList arguments = QApplication::arguments();
  const QString& program_name = arguments.isEmpty() ? QString() : arguments.first();
  if (!Solarus::QuestFiles::open_quest(program_name.toStdString(), quest_path.toStdString())) {
    return;
  }

  const std::string file_name = "settings.dat";
  Solarus::Settings solarus_settings;
  if (Solarus::QuestFiles::data_file_exists(file_name)) {
    // First, load the existing settings.dat to preserve unmodified values.
    solarus_settings.load(file_name);
  }

  QVariant video_mode = value("quest_video_mode");
  if (video_mode.isValid()) {
    solarus_settings.set_string(
          Solarus::Settings::key_video_mode, video_mode.toString().toStdString());
  }

  QVariant fullscreen = value("quest_fullscreen");
  if (fullscreen.isValid()) {
    solarus_settings.set_boolean(
          Solarus::Settings::key_fullscreen, fullscreen.toBool());
  }

  QVariant sound_volume = value("quest_sound_volume");
  if (sound_volume.isValid()) {
    solarus_settings.set_integer(
          Solarus::Settings::key_sound_volume, sound_volume.toInt());
  }

  QVariant music_volume = value("quest_music_volume");
  if (music_volume.isValid()) {
    solarus_settings.set_integer(
          Solarus::Settings::key_music_volume, music_volume.toInt());
  }

  QVariant language = value("quest_language");
  if (language.isValid()) {
    solarus_settings.set_string(
          Solarus::Settings::key_language, language.toString().toStdString());
  }

  QVariant joypad_enabled = value("quest_joypad_enabled");
  if (joypad_enabled.isValid()) {
    solarus_settings.set_boolean(
          Solarus::Settings::key_joypad_enabled, joypad_enabled.toBool());
  }

  solarus_settings.save(file_name);
}

}
