# Under Windows, the data are installed in $(ZSDX_INSTALL_PATH)/data
# instead of ${ZSDX_INSTALL_PATH}/share/zsdx

if (NOT WIN32)
   set (ZSDX_INSTALL_DATA_PATH share/zsdx)
else (NOT WIN32)
   set (ZSDX_INSTALL_DATA_PATH data)
endif (NOT WIN32)

#INSTALL(FILES
#	project_db.zsd
#	DESTINATION ${ZSDX_INSTALL_DATA_PATH}
#)

INSTALL(DIRECTORY
	images
	maps
	music
	sounds
	sprites
	tilesets
	DESTINATION ${ZSDX_INSTALL_DATA_PATH}
	PATTERN ".svn" EXCLUDE
	PATTERN "*~" EXCLUDE
	PATTERN "*.old" EXCLUDE
)
