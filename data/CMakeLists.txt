file(GLOB lua_source_files maps/*.lua)
set(lua_bin_files "")
foreach(lua_source_file ${lua_source_files})
  add_custom_command(
    OUTPUT ${lua_source_file}c
    MAIN_DEPENDENCY ${lua_source_file}
    COMMAND luac -o ${lua_source_file}c ${lua_source_file})
  set(lua_bin_files ${lua_bin_files} ${lua_source_file}c)
endforeach(lua_source_file ${lua_source_files})

add_custom_target(
  data_archive
  ALL
  COMMENT "Building data archive"
  DEPENDS ${lua_bin_files}
  COMMAND zip -q ../data.solarus musics/*.spc sounds/*.wav sprites/*/*.png sprites/*/*.data text/*.dat text/*.ttf text/*.fon tilesets/*.png tilesets/*.dat maps/*.dat maps/*.luac maps/dungeons/*.png maps/dungeons/*.dat
)

