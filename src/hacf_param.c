/*
 * Name        : hacf_param.c
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
#include "hacf_param.h"

hacf_param_list_t *create_hacf_param_list(apr_pool_t *pool) {
	hacf_param_list_t *list = apr_pcalloc(pool, sizeof(*list));
	list->pool = pool;
	return list;
}

hacf_param_t *create_hacf_param(apr_pool_t *pool, char *key, uint32_t hash) {
	hacf_param_t *param = apr_pcalloc(pool, sizeof(*param));
	param->key = key;
	param->hash = hash;
	param->collisions = 0;
	return param;
}

int add_hacf_param(hacf_param_list_t *list, hacf_param_t *param) {
	int collisions = 0;
	if (list->first) {
		hacf_param_t *current = list->first;
		while (current->hash < param->hash) {
			if (current->next) {
				current = current->next;
			} else {
				break;
			}
		}
		if (current->hash == param->hash) {
			if (strcmp(current->key, param->key) != 0) {
				collisions = current->collisions += 1;
			}
		} else {
			if (current->hash < param->hash) {
				param->next = current->next;
				param->previous = current;
				if(param->next) {
					param->next->previous = param;
				}
				current->next = param;
			} else {
				param->next = current;
				param->previous = current->previous;
				current->previous = param;
				if(param->previous) {
					param->previous->next = param;
				} else {
					list->first = param;
				}
			}
		}
	} else {
		list->first = param;
	}
	return collisions;
}

