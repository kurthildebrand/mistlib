/************************************************************************************************//**
 * @file		map.c
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
 * @license		Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * 				file except in compliance with the License. You may obtain a copy of the License at
 *
 * 				http://www.apache.org/licenses/LICENSE-2.0
 *
 * 				Unless required by applicable law or agreed to in writing, software distributed under
 * 				the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * 				ANY KIND, either express or implied. See the License for the specific language
 * 				governing permissions and limitations under the License.
 *
 * @brief		This data structure stores key value pairs. The structure of the entries placed
 * 				placed into the map must contain a lookup_t key as the first field for proper
 * 				operation.
 *
 * @desc		Entries are stored in a sorted array. This means that insertions may need to move all
 * 				entries in the array but retrievals can be performed efficiently using the binary
 * 				search algorithm.
 *
 ***************************************************************************************************/
#include "compare.h"
#include "entry.h"
#include "list.h"
#include "map.h"
#include "search.h"


/* Inline Function Instances --------------------------------------------------------------------- */
extern bool        map_init      (Map*, void*, unsigned, unsigned, unsigned, ICompare);
extern void        map_const_init(Map*, const void*, unsigned, unsigned, ICompare);
extern bool        map_init_range(Map*, Range, unsigned, ICompare);
extern Map         make_map      (void*, unsigned, unsigned, unsigned, ICompare);
extern Map         make_const_map(const void*, unsigned, unsigned, ICompare);
extern Map         map_from_range(Range, unsigned, ICompare);

extern void        map_clear     (Map*);
extern Range*      map_range     (const Map*);
extern Key         map_key       (const Map*);
extern unsigned    map_size      (const Map*);
extern unsigned    map_elemsize  (const Map*);
extern unsigned    map_count     (const Map*);
extern bool        map_empty     (const Map*);
extern bool        map_full      (const Map*);
extern const void* map_entry     (const Map*, unsigned);

extern bool        map_find      (const Map*, const void*, ICompare, Entry*);
extern bool        map_remove    (Map*, unsigned);


/* map_put **************************************************************************************//**
 * @brief		Puts a key value pair into a map.
 * @param[in]	m: the map to place a new key value pair into.
 * @param[in]	in: the new key value pair to insert. Expects the entry's key to be already set.
 * @retval		true if the key value pair was inserted correctly.
 * @retval		false if the map is full or an key value pair with a identical key already exists in
 * 				the map. */
bool map_put(Map* m, const void* in)
{
	void* ptr = map_reserve(m, in, m->compare);

	if(ptr)
	{
		memmove(ptr, in, map_elemsize(m));
		return true;
	}
	else
	{
		return false;
	}
}


/* map_reserve **********************************************************************************//**
 * @brief		Reserves an entry in the map for an element with the specified key.
 * @warning		The user must set the key of the entry after calling map_reserve.
 * @param[in]	m: the map to reserve a key value pair from.
 * @param[in]	key: the key of the entry.
 * @param[in]	c: comparison callback which compares the specified key with entries in the map. This
 * 				callback is called compare(key, <map entries>). */
void* map_reserve(Map* m, const void* key, ICompare c)
{
	Entry e;

	/* Fail early if the map is already full. Also, prevent duplicate keys from being inserted into
	 * the map. */
	if(!map_full(m) && !map_find(m, key, c, &e))
	{
		return list_reserve(&m->list, eidx(&e));
	}
	else
	{
		return 0;
	}
}


/* map_replace **********************************************************************************//**
 * @brief		Replaces an existing entry with a new entry that has an identical key.
 * @param[in]	m: the map to replace a key value pair.
 * @param[in]	in: the new entry with the key already set.
 * @retval		true if the key value pair was replaced successfully.
 * @retval		false if the key value pair was not found in the map. */
bool map_replace(Map* m, const void* in)
{
	Entry e;

	if(in && map_find(m, in, m->compare, &e))
	{
		memmove(eptr(&e), in, map_elemsize(m));
		return true;
	}
	else
	{
		return false;
	}
}


/******************************************* END OF FILE *******************************************/
