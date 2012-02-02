/*
 * Name        : hacf_param.h
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
#ifndef HACF_PARAM_H
#define HACF_PARAM_H 1
#include <sys/types.h>
#include "apr_pools.h"

typedef struct hacf_param_t hacf_param_t;

struct hacf_param_t {
	uint32_t hash;
	int collisions;
    char *key;
    hacf_param_t *previous;
    hacf_param_t *next;
};

typedef struct hacf_param_list_t hacf_param_list_t;

struct hacf_param_list_t {
	apr_pool_t *pool;
	hacf_param_t *first;
};

extern hacf_param_t *create_hacf_param(apr_pool_t *pool, char *key, uint32_t hash);
extern hacf_param_list_t *create_hacf_param_list(apr_pool_t *pool);

/**
 * add an item to the list ordered by hashkey,
 * if an item with the same hashkey exists, check if the value is also equal
 * if the value is equal ignore the item (multi valued parameter),
 * if the value is not equal we have a collision.
 *
 * return the collision count for the inserted item
 */
extern int add_hacf_param(hacf_param_list_t *list, hacf_param_t *param);

#endif
