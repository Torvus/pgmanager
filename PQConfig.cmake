function(_PQ_FIND_LIBRARIES _out_libraries)
	SET(_lib_dir)
	EXECUTE_PROCESS(COMMAND ${PQ_CONFIG_EXECUTABLE} --libdir
	                OUTPUT_VARIABLE _lib_dir)
	STRING(REGEX REPLACE "^(.*)\n$" "\\1" _lib_dir ${_lib_dir})
	SET(${_out_libraries} ${_lib_dir}/libpq.so)
	SET(${_out_libraries} ${${_out_libraries}} PARENT_SCOPE)
endfunction()

function(_PQ_FIND_INCLUDE_DIR _out_includedir)
	SET(_include_dir)
	EXECUTE_PROCESS(COMMAND ${PQ_CONFIG_EXECUTABLE} --includedir
	                OUTPUT_VARIABLE _include_dir)
	STRING(REGEX REPLACE "^(.*)\n$" "\\1" _include_dir ${_include_dir})
	SET(${_out_includedir} ${_include_dir})
	SET(${_out_includedir} ${${_out_includedir}} PARENT_SCOPE)
endfunction()

FIND_PROGRAM(PQ_CONFIG_EXECUTABLE pg_config)

include(FindPackageHandleStandardArgs)

SET(PQ_FOUND)
SET(PQ_INCLUDE_DIRS)
SET(PQ_LIBRARIES)

IF(PQ_CONFIG_EXECUTABLE)
	_PQ_FIND_LIBRARIES(PQ_LIBRARIES)
	_PQ_FIND_INCLUDE_DIR(PQ_INCLUDE_DIRS)
ENDIF(PQ_CONFIG_EXECUTABLE)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PQ DEFAULT_MSG PQ_LIBRARIES PQ_INCLUDE_DIRS)

