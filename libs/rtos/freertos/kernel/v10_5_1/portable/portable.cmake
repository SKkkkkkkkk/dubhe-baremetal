if($CACHE{CORE} STREQUAL "M3")
	set(portable_src
		${CMAKE_CURRENT_LIST_DIR}/GCC/ARM_CM3/port.c
	)

	set(portable_inc
		${CMAKE_CURRENT_LIST_DIR}/GCC/ARM_CM3
	)

elseif($CACHE{CORE} STREQUAL "A55")
	set(portable_src
		${CMAKE_CURRENT_LIST_DIR}/GCC/ARM_CA53_64_BIT/port.c
		${CMAKE_CURRENT_LIST_DIR}/GCC/ARM_CA53_64_BIT/portASM.S
	)

	set(portable_inc
		${CMAKE_CURRENT_LIST_DIR}/GCC/ARM_CA53_64_BIT
	)

else()
	message(FATAL_ERROR "unsupport freertos kernel port!")
endif()


set(portable_src
	${portable_src}
	${CMAKE_CURRENT_LIST_DIR}/MemMang/heap_4.c
)