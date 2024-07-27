set(BINARY "server")

file(GLOB_RECURSE src "*.cpp" "*.h")

add_executable(${BINARY} ${src})

add_custom_command(
    TARGET ${BINARY}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy $<TARGET_FILE:${BINARY}> ${CMAKE_BINARY_DIR}/target/${BINARY}
)