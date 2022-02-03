/************************************************************************************************//**
 * @file		test_json.c
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
 ***************************************************************************************************/
#include "json.h"
#include "tharness.h"


/* Private Functions ----------------------------------------------------------------------------- */
void print_json(const JsonToken*);


TEST(test_flat_json)
{
	char json[] = {
		"{\"data\": \"Click Here\","
		"\"size\": 36,"
		"\"style\": \"bold\","
		"\"name\": \"text1\","
		"\"hOffset\": 250,"
		"\"vOffset\": 100,"
		"\"alignment\": \"center\","
		"\"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\",}"
	};

	Buffer b = make_buffer(json, sizeof(json), sizeof(json));

	JsonToken base;
	JsonToken elem;

	json_init(&b, &base);

	EXPECT(json_read(&b, &base, &elem)); 	/* data */
	EXPECT(string_cmp(elem.key, MAKE_STRING("data")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("Click Here")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* size */
	EXPECT(string_cmp(elem.key, MAKE_STRING("size")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("36")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* style */
	EXPECT(string_cmp(elem.key, MAKE_STRING("style")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("bold")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* name */
	EXPECT(string_cmp(elem.key, MAKE_STRING("name")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("text1")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* hOffset */
	EXPECT(string_cmp(elem.key, MAKE_STRING("hOffset")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("250")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* vOffset */
	EXPECT(string_cmp(elem.key, MAKE_STRING("vOffset")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("100")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* alignment */
	EXPECT(string_cmp(elem.key, MAKE_STRING("alignment")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("center")) == 0);
	print_json(&elem);

	EXPECT(json_read(&b, &base, &elem)); 	/* onMouseUp */
	EXPECT(string_cmp(elem.key, MAKE_STRING("onMouseUp")) == 0 &&
	       string_cmp(elem.value, MAKE_STRING("sun1.opacity = (sun1.opacity / 100) * 90;")) == 0);
	print_json(&elem);

	EXPECT(!json_read(&b, &base, &elem));

	EXPECT(string_cmp(base.value, MAKE_STRING(json)) == 0);
	print_json(&base);
}


TEST(test_hierarchical_json)
{
	char json[] = {
		"{\"widget\": {"
			"\"debug\": \"on\","
			"\"window\": {"
				"\"title\": \"Sample Konfabulator Widget\","
				"\"name\": \"main_window\","
				"\"width\": 500,"
				"\"height\": 500"
			"},"
			"\"image\": {"
				"\"src\": \"Images/Sun.png\","
				"\"name\": \"sun1\","
				"\"hOffset\": 250,"
				"\"vOffset\": 250,"
				"\"alignment\": \"center\""
			"},"
			"\"text\": {"
				"\"data\": \"Click Here\","
				"\"size\": 36,"
				"\"style\": \"bold\","
				"\"name\": \"text1\","
				"\"hOffset\": 250,"
				"\"vOffset\": 100,"
				"\"alignment\": \"center\","
				"\"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\""
			"}"
		"}}"
	};

	Buffer b = make_buffer(json, sizeof(json), sizeof(json));

	JsonToken base, level1, level2, level3;

	/* 	widget:                                                      */
	/* 		debug: on,                                               */
	/* 		window:                                                  */
	/* 			title: Sample Konfabulator Widget,                   */
	/* 			name: main_window,                                   */
	/* 			width: 500,                                          */
	/* 			height: 500                                          */
	/* 		image:                                                   */
	/* 			src: Images/Sun.png,                                 */
	/* 			name: sun1,                                          */
	/* 			hOffset: 250,                                        */
	/* 			vOffset: 250,                                        */
	/* 			alignment: center                                    */
	/* 		text:                                                    */
	/* 			data: Click Here,                                    */
	/* 			size: 36,                                            */
	/* 			style: bold,                                         */
	/* 			name: text1,                                         */
	/* 			hOffset: 250,                                        */
	/* 			vOffset: 100,                                        */
	/* 			alignment: center,                                   */
	/* 			onMouseUp: sun1.opacity = (sun1.opacity / 100) * 90; */

	json_init(&b, &base);
	/* 	widget:                                                      */

	EXPECT( json_read(&b, &base,   &level1));
	EXPECT(string_cmp(json_key  (&level1), MAKE_STRING("widget")) == 0);

	/* 		debug: on,                                               */
	EXPECT( json_read(&b, &level1, &level2));
	EXPECT(string_cmp(json_key  (&level2), MAKE_STRING("debug")) == 0);
	EXPECT(string_cmp(json_value(&level2), MAKE_STRING("on")) == 0);

	/* 		window:                                                  */
	EXPECT( json_read(&b, &level1, &level2));
	EXPECT(string_cmp(json_key  (&level2), MAKE_STRING("window")) == 0);

	/* 			title: Sample Konfabulator Widget,                   */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("title")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("Sample Konfabulator Widget")) == 0);

	/* 			name: main_window,                                   */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("name")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("main_window")) == 0);

	/* 			width: 500,                                          */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("width")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("500")) == 0);

	/* 			height: 500                                          */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("height")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("500")) == 0);

	EXPECT(!json_read(&b, &level2, &level3));
	/* 		image:                                                   */
	EXPECT( json_read(&b, &level1, &level2));
	EXPECT(string_cmp(json_key  (&level2), MAKE_STRING("image")) == 0);
	/* 			src: Images/Sun.png,                                 */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("src")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("Images/Sun.png")) == 0);
	/* 			name: sun1,                                          */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("name")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("sun1")) == 0);
	/* 			hOffset: 250,                                        */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("hOffset")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("250")) == 0);
	/* 			vOffset: 250,                                        */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("vOffset")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("250")) == 0);
	/* 			alignment: center                                    */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("alignment")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("center")) == 0);

	EXPECT(!json_read(&b, &level2, &level3));
	/* 		text:                                                    */
	EXPECT( json_read(&b, &level1, &level2));
	EXPECT(string_cmp(json_key  (&level2), MAKE_STRING("text")) == 0);
	/* 			data: Click Here,                                    */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("data")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("Click Here")) == 0);
	/* 			size: 36,                                            */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("size")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("36")) == 0);
	/* 			style: bold,                                         */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("style")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("bold")) == 0);
	/* 			name: text1,                                         */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("name")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("text1")) == 0);
	/* 			hOffset: 250,                                        */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("hOffset")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("250")) == 0);
	/* 			vOffset: 100,                                        */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("vOffset")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("100")) == 0);
	/* 			alignment: center,                                   */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("alignment")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("center")) == 0);
	/* 			onMouseUp: sun1.opacity = (sun1.opacity / 100) * 90; */
	EXPECT( json_read(&b, &level2, &level3));
	EXPECT(string_cmp(json_key  (&level3), MAKE_STRING("onMouseUp")) == 0);
	EXPECT(string_cmp(json_value(&level3), MAKE_STRING("sun1.opacity = (sun1.opacity / 100) * 90;")) == 0);

	EXPECT(!json_read(&b, &level2, &level3));

	EXPECT(!json_read(&b, &level1, &level2));
	EXPECT(!json_read(&b, &base, &level1));
}


void print_json(const JsonToken* t)
{
	PRINT("%.*s: %.*s\n",
		string_length(&t->key), string_start(&t->key),
		string_length(&t->value), string_start(&t->value));
}


void test_json(void)
{
	tharness_run(test_flat_json);
	tharness_run(test_hierarchical_json);
}


/******************************************* END OF FILE *******************************************/
