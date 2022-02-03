cmake_minimum_required(VERSION 3.13.1)

project(run-mistlib-tests)

add_executable(run-mistlib-tests ${SOURCE_FILES})

add_subdirectory(tharness)

target_include_directories(run-mistlib-tests PRIVATE ./)
target_include_directories(run-mistlib-tests PRIVATE tharness)

set_property(TARGET run-mistlib-tests PROPERTY CMAKE_BUILD_TYPE Debug)

target_sources(run-mistlib-tests PRIVATE
	main.c
	test_array.c
	test_bits.c
	test_buffer.c
	test_byteorder.c
	test_calc.c
	test_heap.c
	test_ieee_802_15_4.c
	test_insertsort.c
	test_ipv6.c
	test_json.c
	test_linked.c
	test_list.c
	test_map.c
	test_matrix.c
	test_order.c
	test_pool.c
	test_queue.c
	test_range.c
	test_ringbuffer.c
	test_search.c
	test_selsort.c
	test_stack.c
)

target_link_libraries(run-mistlib-tests mistlib)
target_link_libraries(run-mistlib-tests tharness)
