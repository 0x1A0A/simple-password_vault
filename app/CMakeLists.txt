file(
	GLOB_RECURSE APP_SRCS
	"${CMAKE_CURRENT_LIST_DIR}/src/*.c"
	)

add_executable(ppsm ${APP_SRCS})

target_link_libraries(ppsm df_password_man df_crypto editline)
target_include_directories(ppsm PRIVATE ${CMAKE_HOME_DIRECTORY}/df_psm/src)
