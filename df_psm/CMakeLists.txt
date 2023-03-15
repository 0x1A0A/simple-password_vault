file(GLOB SRC_PSM_FILE
	${CMAKE_CURRENT_LIST_DIR}/src/*.c
	${CMAKE_CURRENT_LIST_DIR}/src/*.h
	${CMAKE_CURRENT_LIST_DIR}/*.h
)

file(GLOB SRC_CRYPTO_FILE
	${CMAKE_CURRENT_LIST_DIR}/src/crypto/*.c
	${CMAKE_CURRENT_LIST_DIR}/src/crypto/*.h
)

add_library(
	df_password_man STATIC
	${SRC_PSM_FILE}
)

add_library(
	df_crypto STATIC
	${SRC_CRYPTO_FILE}
)