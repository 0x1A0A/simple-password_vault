file(
	GLOB_RECURSE APP_SRCS
	"${CMAKE_CURRENT_LIST_DIR}/src/*.c"
	)

find_package(Curses)

add_executable(ppsm ${APP_SRCS})

target_link_libraries(ppsm df_password_man df_crypto ${CURSES_LIBRARIES})
target_include_directories(ppsm PRIVATE ${CMAKE_HOME_DIRECTORY}/df_psm/src PRIVATE ${CURSES_INCLUDE_DIRS})
