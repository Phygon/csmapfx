cmake_minimum_required(VERSION 2.8)
project (csmapfx CXX)
include (FindPkgConfig)

pkg_search_module(FOX REQUIRED fox>=1.6)
find_package(Boost REQUIRED)
find_package(PNG REQUIRED)

if (WIN32)
# vcpkg install gettext
    find_package(unofficial-gettext CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::gettext::libintl)
endif (WIN32)

set (SOURCES
	calc.cpp
	commands.cpp
	csmap.cpp
	datafile.cpp
	exportdlg.cpp
	FXFileDialogEx.cpp
	fxhelper.cpp
	FXMenuSeparatorEx.cpp
	FXSplitterEx.cpp
	imageio.cpp
	infodlg.cpp
	main.cpp
	mapcanvas.cpp
	map.cpp
	messages.cpp
	regioninfos.cpp
	regionlist.cpp
	scout.cpp
	searchdlg.cpp
	statistics.cpp
	statsinfos.cpp
	symbols.cpp
	tradeinfos.cpp
	translator.cpp
	unitlist.cpp
	tinyjs/TinyJS.cpp
	tinyjs/TinyJS_Functions.cpp
	tinyjs/TinyJS_MathFunctions.cpp
)
include_directories(
	${PNG_INCLUDE_DIRS}
	${FOX_INCLUDE_DIRS}
)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 11)
target_link_libraries(${PROJECT_NAME}
	${PNG_LIBRARIES}
	${FOX_LIBRARIES}
)

