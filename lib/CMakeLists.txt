add_library(lib task.cpp task.h)
target_include_directories(lib PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/lib")