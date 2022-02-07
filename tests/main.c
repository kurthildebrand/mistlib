#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "calc.h"
#include "byteorder.h"

#include "tharness.h"
#include "test_array.h"
#include "test_bits.h"
#include "test_buffer.h"
#include "test_byteorder.h"
#include "test_calc.h"
#include "test_heap.h"
#include "test_icmp6.h"
#include "test_ieee_802_15_4.h"
#include "test_insertsort.h"
#include "test_ipv6.h"
#include "test_json.h"
#include "test_linked.h"
#include "test_list.h"
#include "test_lowpan.h"
#include "test_map.h"
#include "test_ndp.h"
#include "test_order.h"
#include "test_pool.h"
#include "test_queue.h"
#include "test_range.h"
#include "test_ringbuffer.h"
#include "test_search.h"
#include "test_selsort.h"
#include "test_stack.h"

#include "range.h"

typedef struct {
	uint64_t a;
	uint32_t b;
	uint16_t c;
	uint8_t  d;
} Test;

int main()
{
	srand(time(0));

	tharness_init(false);
	// tharness_init(true);

	test_range();
	test_order();
	test_search();
	test_calc();
	test_buffer();
	test_byteorder();
	test_insertsort();
	test_selsort();
	test_array();
	test_linked();
	test_list();
	test_stack();
	test_ringbuffer();
	test_heap();
	test_map();
	test_pool();
	test_queue();
 	test_bits();

 	test_ipv6();
 	test_icmp6();
	test_ndp();
 	test_ieee_802_15_4();
//// //	test_lowpan();

	test_json();

	return tharness_results();


//	printf("%d\n", calc_wrapdiff_i32(10, 42, 60000));
//	printf("%d\n", calc_wrapdiff_i32(20, 42, 60000));
//	printf("%d\n", calc_wrapdiff_i32(40, 42, 60000));
//	printf("%d\n", calc_wrapdiff_i32(60, 42, 60000));

//	int x = 10;
//	int asn = 290;
//	int numslots = 100;
//
//
//	printf("%d\n", calc_wrapdiff_i32(x, asn, numslots));
//	printf("%d\n", calc_wrapdiff_i32(x, asn % numslots, numslots));


//	Test test;
//	ntoh(&test.a, 0x1234);
//	ntoh(&test.b, 0x5678);
//	ntoh(&test.c, 0x9ABC);
//	ntoh(&test.d, 0xDE);
//
//	printf("hton(%llX) = %llX\n", test.a, hton(&test.a));
//	printf("hton(%lX) = %lX\n", test.b, hton(&test.b));
//	printf("hton(%X) = %X\n", test.c, hton(&test.c));
//	printf("hton(%X) = %X\n", test.d, hton(&test.d));

	return 0;
}
