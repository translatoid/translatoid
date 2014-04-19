# Project Needs a name ofcourse
project(translatoid)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
# Find the required Libaries
find_package(KDE4 REQUIRED)
find_package(QJSON REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QJSON_INCLUDE_DIR}
   )
 
# We add our source code here

set(ktranslatoid_SRCS ktranslatoid.cpp   languagemodel.cpp reminder.cpp)
kde4_add_ui_files(ktranslatoid_SRCS  form/languageform.ui form/reminderform.ui)
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_translatoid ${ktranslatoid_SRCS})



target_link_libraries(plasma_applet_translatoid 
                      ${KDE4_PLASMA_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_SOLID_LIBS}
                      ${QT_QTNETWORK_LIBRARY}
                      ${QT_QTSQL_LIBRARY}
                      ${QJSON_LIBRARIES}

                   )

install(TARGETS plasma_applet_translatoid
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-translatoid.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
	
install(FILES translatoid.db DESTINATION ${DATA_INSTALL_DIR}/translatoid)

#/usr/share/kde4/services/
#/usr/lib/kde4/

add_subdirectory(icons)
add_subdirectory(kbflags)
add_subdirectory(cmake)
add_subdirectory(po)
