if(NOT DEFINED DIR)
    set(DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()

if(NOT DEFINED RELATIVE_DIR)
    set(RELATIVE_DIR "../../..")
endif()

set(DIRS_TO_CLEAN
    "${DIR}"
    "${DIR}/.."
    "${DIR}/${RELATIVE_DIR}/DING_OS"
    "${DIR}/${RELATIVE_DIR}/OSAL/linux_kernel"
    "${DIR}/${RELATIVE_DIR}/OSAL_TEST"
)

foreach(CLEAN_DIR ${DIRS_TO_CLEAN})
    get_filename_component(ABS_CLEAN_DIR "${CLEAN_DIR}" ABSOLUTE)
    file(GLOB FILES_TO_REMOVE "${ABS_CLEAN_DIR}/*.o" "${ABS_CLEAN_DIR}/*.cmd")
    if(FILES_TO_REMOVE)
        file(REMOVE ${FILES_TO_REMOVE})
    endif()
endforeach()
