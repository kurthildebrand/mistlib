/************************************************************************************************//**
 * @file		map.h
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
 * 				placed into the map must contain a Key key as the first field for proper operation.
 *
 * @desc		Entries are stored in a sorted array. This means that insertions may need to
 * 				move all entries in the array but retrievals can be performed efficiently using the
 * 				binary search algorithm.
 *
 ***************************************************************************************************/
#ifndef MAP_H
#define MAP_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <stdbool.h>

#include "list.h"
#include "search.h"
#include "utils.h"


/* Public Macros --------------------------------------------------------------------------------- */
/* MAKE_MAP *************************************************************************************//**
 * @brief		Initializes a Map at compile time. This macro may be called with a varying number of
 *				arguments.
 * @param[in]	k: the key of the Map.
 * @param[in]	d: pointer to the Map's data.
 * @param[in]	s: total number of entries in the Map.
 * @param[in]	e: the number of bytes per entry.
 * @param[in]	c: comparison callback which compares entries in the map.
 * @desc		MAKE_MAP(d,c): Initializes a Map to point to a raw array and with a comparison
 * 					callback c. The size and element size are automatically set using sizeof. In
 * 					addition, the Map's count is set to size to create a full Map. The macro performs
 * 					the following operation:
 *
 *					const int data[] = {
 *						10, 20, 30, 40, 50
 *					};
 *
 *					const Map m1 = {
 *						.entries = data,
 *						.elemsize = sizeof(data[0]),
 *						.size = sizeof(data) / sizeof(data[0]),
 *						.count = sizeof(data) / sizeof(data[0}),
 *					};
 *
 *					const Map m2 = MAKE_MAP(data);
 *
 *				MAKE_MAP(k,d,c): Performs the same initialization as MAKE_MAP(d) but also sets the
 *					Map's key.
 *
 *				MAKE_MAP(d,s,e,c): Sets the Map's pointer, size, element size, and comparison
 * 					callback. The Map's count is set to size creating a full Map.
 *
 *				MAKE_MAP(k,d,s,e,c): Sets the Map's key, pointer, size, element size, and comparison
 * 					callback. */
#define MAKE_MAP(...)			APPEND_NARGS(MAKE_MAP, __VA_ARGS__)
#define MAKE_MAP2(d,c)			MAKE_MAP4((d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]), (c))
#define MAKE_MAP3(k,d,c)		MAKE_MAP5((k), (d), sizeof((d))/sizeof((d)[0]), sizeof((d)[0]), (c))
#define MAKE_MAP4(d,s,e,c)		(Map){ .list=MAKE_LIST3(    (d),(s),(e)) }, .compare=(c) }
#define MAKE_MAP5(k,d,s,e,c)	(Map){ .list=MAKE_LIST4((k),(d),(s),(e)) }, .compare=(c) }


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	List list;
	ICompare compare;
} Map;


/* Public Functions ------------------------------------------------------------------------------ */
inline bool        map_init      (Map*, void*, unsigned, unsigned, unsigned, ICompare);
inline void        map_const_init(Map*, const void*, unsigned, unsigned, ICompare);
inline bool        map_init_range(Map*, Range, unsigned, ICompare);
inline Map         make_map      (void*, unsigned, unsigned, unsigned, ICompare);
inline Map         make_const_map(const void*, unsigned, unsigned, ICompare);
inline Map         map_from_range(Range, unsigned, ICompare);

inline void        map_clear     (Map* m)       {        list_clear(&m->list);    }
inline Range*      map_range     (const Map* m) { return list_range(&m->list);    }
inline Key         map_key       (const Map* m) { return list_key(&m->list);      }
inline unsigned    map_size      (const Map* m) { return list_size(&m->list);     }
inline unsigned    map_elemsize  (const Map* m) { return list_elemsize(&m->list); }
inline unsigned    map_count     (const Map* m) { return list_count(&m->list);    }
inline bool        map_empty     (const Map* m) { return list_empty(&m->list);    }
inline bool        map_full      (const Map* m) { return list_full(&m->list);     }
inline const void* map_entry     (const Map*, unsigned);

       bool        map_put       (Map*, const void*);
       void*       map_reserve   (Map*, const void*, ICompare);
       bool        map_replace   (Map*, const void*);
inline bool        map_find      (const Map*, const void*, ICompare, Entry*);
inline bool        map_remove    (Map* m, unsigned idx) { return list_remove(&m->list, idx); }


/* map_init *************************************************************************************//**
 * @brief		Initializes a map to point to the specified data.
 * @param[in]	m: the map to initialize.
 * @param[in]	ptr: the buffer to hold map entries.
 * @param[in]	count: the current number of entries in the list.
 * @param[in]	size: the total number of entries in the map.
 * @param[in]	elemsize: the size of an entry in bytes.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline bool map_init(Map* m, void* ptr, unsigned count, unsigned size, unsigned elemsize, ICompare c)
{
	if(list_init(&m->list, ptr, count, size, elemsize))
	{
		m->compare = c;
		return true;
	}
	else
	{
		return false;
	}
}


/* map_const_init *******************************************************************************//**
 * @brief		Initializes a map to point to the specified const data. The map is initialized full.
 * @param[in]	m: the map to initialize.
 * @param[in]	ptr: pointer to the map's entry buffer.
 * @param[in]	size: the total number of entries in the map.
 * @param[in]	elemsize: the size of an entry in bytes.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline void map_const_init(Map* m, const void* ptr, unsigned size, unsigned elemsize, ICompare c)
{
	list_const_init(&m->list, ptr, size, elemsize);
	m->compare = c;
}


/* map_init_range *******************************************************************************//**
 * @brief		Initializes a map from the specified range.
 * @param[in]	m: the map to initialize.
 * @param[in]	r: the range of entries in the map.
 * @param[in]	count: the current number of entries in the list.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline bool map_init_range(Map* m, Range r, unsigned count, ICompare c)
{
	if(list_init_range(&m->list, r, count))
	{
		m->compare = c;
		return true;
	}
	else
	{
		return false;
	}
}


/* make_map *************************************************************************************//**
 * @brief		Creates a map pointing to the specified data.
 * @param[in]	ptr: the map's entries.
 * @param[in]	count: the current number of entries in the map.
 * @param[in]	size: the total number of entries in the map.
 * @param[in]	elemsize: the size of an entry in bytes.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline Map make_map(void* ptr, unsigned count, unsigned size, unsigned elemsize, ICompare c)
{
	return (Map){ .list = make_list(ptr, count, size, elemsize), .compare = c };
}


/* make_const_map *******************************************************************************//**
 * @brief		Creates a map pointing to the specified const data. The map is initialized full.
 * @param[in]	ptr: the map's entries.
 * @param[in]	size: the total number of entries in the map.
 * @param[in]	elemsize: the size of an entry in bytes.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline Map make_const_map(const void* ptr, unsigned size, unsigned elemsize, ICompare c)
{
	return (Map){ .list = make_const_list(ptr, size, elemsize), .compare = c };
}


/* map_from_range *******************************************************************************//**
 * @brief		Creates a map from a range.
 * @param[in]	r: the range representing the map's entries.
 * @param[in]	count: the current number of entries in the map.
 * @param[in]	c: comparison callback which compares entries in the map. */
inline Map map_from_range(Range r, unsigned count, ICompare c)
{
	return (Map){
		.list    = list_from_range(r, count),
		.compare = c,
	};
}


/* map_entry ************************************************************************************//**
 * @brief		Returns a pointer to the element at index i. Returns null if index i is out of
 *				range. */
inline const void* map_entry(const Map* m, unsigned idx)
{
	return list_entry(&m->list, idx);
}


/* map_find *************************************************************************************//**
 * @brief		Searches a map for the specified key. Returns true if the key is found.
 * @param[in]	m: the map to operate on.
 * @param[in]	key: the key to search for.
 * @param[in]	comp: comparison callback which compares the key with entries in the map. If null,
 * 				map_find will use the map's comparison callback.
 * @param[out]	entry: the entry to the value if found. If not found, the index will be set to the
 * 				location to insert a value to maintain the map's sorting.
 * @retval		true if the key was found.
 * @retval		false if the key was not found. */
inline bool map_find(const Map* m, const void* key, ICompare comp, Entry* entry)
{
	Range* r = map_range(m);

	if(!comp)
	{
		return binfind(r, key, m->compare, entry);
	}
	else
	{
		return binfind(r, key, comp, entry);
	}
}


#ifdef __cplusplus
}
#endif

#endif // MAP_H
/******************************************* END OF FILE *******************************************/
