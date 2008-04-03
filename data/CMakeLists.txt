# Under Windows, the data are installed in $(ZSDX_INSTALL_PATH)/data
# instead of ${ZSDX_INSTALL_PATH}/share/zsdx

if (NOT CMAKE_HOST_WIN32)
   set (ZSDX_INSTALL_DATA_PATH share/zsdx)
else (NOT CMAKE_HOST_WIN32)
   set (ZSDX_INSTALL_DATA_PATH data)
endif (NOT CMAKE_HOST_WIN32)

INSTALL(FILES
	project_db.zsd
	zsdx.ttf
	DESTINATION ${ZSDX_INSTALL_DATA_PATH}
)

INSTALL(DIRECTORY
	images
	maps
	music
	sounds
	sprites
	tilesets
	DESTINATION ${ZSDX_INSTALL_DATA_PATH}
	PATTERN ".svn" EXCLUDE
)
