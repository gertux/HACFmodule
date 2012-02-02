/*
 * Name        : testhacf_param.c
 * Author      : Gert Dewit

 * The Author licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "testall.h"
#include "hacf_param.h"

#define KEY1 "tttttttttttttt"
#define KEY2 "ttttttttv6v6tt"
#define KEY4 "aaaaaaaaaaaaaa"
#define KEY3 "ttttttttaaaaaa"
uint32_t HASH1 = 388908416;
uint32_t HASH2 = 388908416;
uint32_t HASH3 = 4121790048;

static void hacf_param_list_add_test(CuTest *tc) {
	hacf_param_list_t *list = create_hacf_param_list(p);
	hacf_param_t *key1 = create_hacf_param(p, KEY1, HASH1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key1));
	CuAssertPtrNotNull(tc, list->first);
}

static void hacf_param_list_multivalue_test(CuTest *tc) {
	hacf_param_list_t *list = create_hacf_param_list(p);
	hacf_param_t *key11 = create_hacf_param(p, KEY1, HASH1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key11));
	CuAssertPtrNotNull(tc, list->first);
	hacf_param_t *key12 = create_hacf_param(p, KEY1, HASH1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key12));
	CuAssertPtrNull(tc, list->first->next);
}

static void hacf_param_list_collision_test(CuTest *tc) {
	hacf_param_list_t *list = create_hacf_param_list(p);
	hacf_param_t *key1 = create_hacf_param(p, KEY1, HASH1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key1));
	CuAssertPtrNotNull(tc, list->first);
	hacf_param_t *key2 = create_hacf_param(p, KEY2, HASH2);
	CuAssertIntEquals(tc, 1, add_hacf_param(list, key2));
	CuAssertPtrNull(tc, list->first->next);
}

static void hacf_param_list_nocollision_test(CuTest *tc) {
	hacf_param_list_t *list = create_hacf_param_list(p);
	hacf_param_t *key1 = create_hacf_param(p, KEY1, HASH1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key1));
	CuAssertPtrNotNull(tc, list->first);
	hacf_param_t *key3 = create_hacf_param(p, KEY3, HASH3);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key3));
	CuAssertPtrNotNull(tc, list->first->next);
	CuAssertIntEquals(tc, HASH1, list->first->hash);
	CuAssertIntEquals(tc, HASH3, list->first->next->hash);
}

static void hacf_param_list_order_test(CuTest *tc) {
	hacf_param_list_t *list = create_hacf_param_list(p);
	hacf_param_t *key2 = create_hacf_param(p, KEY2, 2);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key2));
	CuAssertPtrNotNull(tc, list->first);
	CuAssertIntEquals(tc, 2, list->first->hash);
	hacf_param_t *key4 = create_hacf_param(p, KEY4, 4);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key4));
	CuAssertPtrNotNull(tc, list->first->next);
	CuAssertIntEquals(tc, 4, list->first->next->hash);
	hacf_param_t *key1 = create_hacf_param(p, KEY1, 1);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key1));
	CuAssertPtrNotNull(tc, list->first);
	CuAssertIntEquals(tc, 1, list->first->hash);
	CuAssertIntEquals(tc, 2, list->first->next->hash);
	CuAssertIntEquals(tc, 4, list->first->next->next->hash);
	hacf_param_t *key3 = create_hacf_param(p, KEY3, 3);
	CuAssertIntEquals(tc, 0, add_hacf_param(list, key3));
	CuAssertPtrNotNull(tc, list->first);
	CuAssertIntEquals(tc, 1, list->first->hash);
	CuAssertIntEquals(tc, 2, list->first->next->hash);
	CuAssertIntEquals(tc, 3, list->first->next->next->hash);
	CuAssertIntEquals(tc, 4, list->first->next->next->next->hash);
}

//typedef struct perf_pair perf_pair;
//struct perf_pair {
//	char *key;
//	uint32_t hash;
//};

//static void hacf_param_list_perf_test(CuTest *tc) {
//	int num_elems = 1024;
//	perf_pair pairs[1024];
//	pairs = (perf_pair)apr_pcalloc(p, num_elems * sizeof(perf_pair));
//	int i;
//	for(i = 0; i < num_elems; i++) {
//		pairs[i]->hash = i;
//		pairs[i]->key = apr_psprintf(p, "key-%d-suf", i);
//	}
//	apr_time_t start = apr_time_now();
//	for(i = 0; i < num_elems; i++) {
//		fprintf(stderr, "key = %s, hash = %d", pairs[i]->key, pairs[i]->hash);
//	}
//	apr_time_t end = apr_time_now();
//	fprintf(stderr, "Performance Test took %ll", end - start);
//}


CuSuite *testhacf_param(void) {
    CuSuite *suite = CuSuiteNew("HACF_Param");

    SUITE_ADD_TEST(suite, hacf_param_list_add_test);
    SUITE_ADD_TEST(suite, hacf_param_list_multivalue_test);
    SUITE_ADD_TEST(suite, hacf_param_list_collision_test);
    SUITE_ADD_TEST(suite, hacf_param_list_nocollision_test);
    SUITE_ADD_TEST(suite, hacf_param_list_order_test);
//    SUITE_ADD_TEST(suite, hacf_param_list_perf_test);

    return suite;
}
