# Link webpage files into the program

# List of pages
SET(WEBPAGE_FILES
	"admin.html"
	"index.html")

# Transform each file into OBJ
foreach(WEBPAGE IN LISTS WEBPAGE_FILES)
	SET(HTML_FILE "${CMAKE_CURRENT_LIST_DIR}/${WEBPAGE}")
	SET(OBJ_FILE "${CMAKE_CURRENT_BINARY_DIR}/${WEBPAGE}.o")

	list(APPEND WEBPAGE_OBJS ${OBJ_FILE})

	set_source_files_properties(${OBJ_FILE}
		PROPERTIES
		GENERATED TRUE
		EXTERNAL_OBJECT TRUE)

	add_custom_command(
		OUTPUT ${OBJ_FILE}
		DEPENDS ${HTML_FILE}
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
		COMMAND "${CMAKE_OBJCOPY}" -I binary -O elf32-avr --rename-section .data=.rodata,alloc,load,readonly,data,contents "${WEBPAGE}" "${OBJ_FILE}"
		VERBATIM)
endforeach()

# Create header with the variables for the webpages
SET(WEBPAGE_HEADER "${CMAKE_BINARY_DIR}/webpages.h")

add_custom_command(
		OUTPUT ${WEBPAGE_HEADER}
		DEPENDS "${WEBPAGE_OBJS}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
		COMMAND "py" "make_header.py" "${WEBPAGE_HEADER}" "${WEBPAGE_FILES}"
		VERBATIM)

# Create library to link with files
add_library(LINK_WEBPAGES STATIC "${WEBPAGE_OBJS}" "${WEBPAGE_HEADER}")

set_target_properties(LINK_WEBPAGES PROPERTIES
  LINKER_LANGUAGE C )