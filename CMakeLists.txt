cmake_minimum_required(VERSION 3.13.1)

project(mistlib VERSION 1.0.0)

add_library(mistlib STATIC ${SOURCES})

set_property(TARGET mistlib PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET mistlib PROPERTY C_STANDARD 11)
set_property(TARGET mistlib PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET mistlib PROPERTY C_EXTENSIONS OFF)

target_compile_definitions(mistlib PUBLIC _ANSI_SOURCE)

target_include_directories(mistlib PRIVATE ./)

target_include_directories(mistlib PUBLIC
	algorithms/
	net/
	net/ip/
	types/
)

target_sources(mistlib PUBLIC
	algorithms/byteorder.c
	algorithms/calc.c
	algorithms/insertsort.c
	algorithms/matrix.c
	algorithms/order.c
	algorithms/search.c
	algorithms/selsort.c
	net/ieee_802_15_4.c
	net/ip/ipv6.c
	net/ip/icmp6.c
	net/ip/ndp.c
	net/lowpan.c
	types/array.c
	types/bits.c
	types/buffer.c
	types/compare.c
	types/entry.c
	types/heap.c
	types/json.c
	types/key.c
	types/linked.c
	types/list.c
	types/map.c
	types/pool.c
	types/queue.c
	types/range.c
	types/ringbuffer.c
	types/stack.c
)
