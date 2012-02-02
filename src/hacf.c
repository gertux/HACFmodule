/*
 * Name        : hacf.c
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
#include <hacf.h>
#include <apr_strings.h>

uint32_t java_32bit_hash(const char *key) {
	uint32_t hash = 0;
	while (*key) {
		hash = 31 * hash + *key++;
	}
	return hash;
}

static inline uint32_t zend_inline_hash_func(const char *arKey, uint nKeyLength)
{
        register uint32_t hash = 5381;

        /* variant with the hash unrolled eight times */
        for (; nKeyLength >= 8; nKeyLength -= 8) {
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
                hash = ((hash << 5) + hash) + *arKey++;
        }
        switch (nKeyLength) {
                case 7: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 6: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 5: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 4: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 3: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 2: hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
                case 1: hash = ((hash << 5) + hash) + *arKey++; break;
                case 0: break;
                default: break;
        }
        return hash;
}


uint32_t php_32bit_hash(const char *key) {
	uint32_t hash = 5381;
	while (*key) {
		hash = (hash << 5 ) + hash + *key++;
	}
	return hash;
}

uint32_t python_32bit_hash(const char *key) {
	uint32_t hash = 5381;
	while (*key) {
		hash = ((hash << 5 ) + hash) ^ *key++;
	}
	return hash;
}

char *get_next_parameter(apr_pool_t *pool, const char *orig_start, apr_size_t *offset) {
	(*offset) = 0;
	const char *current;
	const char *start;
	start = orig_start;
	current = start;

	if (*current == '=') {
		/* continuation can start with a =, skip it */
		(*offset)++;
		current++;
	}

	while (*current && (*current != '=')) {
		(*offset)++;
		current++;
		if (*current == '&') {
			/* this is a continuation, skip over */
			start += (*offset) + 1;
			current++;
			(*offset) = 0;
		}
	}

	if (!*current) {
		return NULL;
	}

	char *key = apr_pstrmemdup(pool, start, (*offset));

	while (*current && (*current != '&')) {
		(*offset)++;
		current++;
	}
	(*offset)++;

	return key;
}

hacf_assert_t *assert_parameters(hacf_assert_t *assert, const char *query, const char *prefix, hacf_conf_t *conf) {
	int parameters = 0;
	apr_size_t offset;
	while (*query) {
		char *key = get_next_parameter(assert->list->pool, query, &offset);
		if (key) {
			if(prefix) {
				char *newkey = apr_pstrcat(assert->list->pool, prefix, key, NULL);
				key = newkey;
			}
			uint32_t hash = conf->hash_function(key);
			hacf_param_t *param = create_hacf_param(assert->list->pool, key, hash);
			int collisions = add_hacf_param(assert->list, param);
			query += offset;
			if (collisions > conf->max_collisions || ++parameters > conf->max_parameters) {
				assert->result = ASSERT_BAD;
				assert->remains = query;
				if(collisions > conf->max_collisions) {
					assert->message = apr_psprintf(assert->list->pool, "To much collisions (>%d)", conf->max_collisions);
				} else {
					assert->message = apr_psprintf(assert->list->pool, "To much parameters (>%d)", conf->max_parameters);
				}
				break;
			}
		} else {
			assert->remains = query;
			break;
		}
	}
	return assert;
}

hacf_assert_t *create_hacf_assert(apr_pool_t *pool) {
	hacf_assert_t *assert = apr_pcalloc(pool, sizeof(*assert));
	assert->result = ASSERT_OK;
	assert->message = "";
	assert->list = create_hacf_param_list(pool);
	return assert;
}

hacf_conf_t *create_hacf_conf(apr_pool_t *pool) {
	hacf_conf_t *conf = apr_pcalloc(pool, sizeof(*conf));
	conf->language = NO_LANGUAGE;
	conf->max_collisions = DEFAULT_MAX_COLLISIONS;
	conf->max_parameters = DEFAULT_MAX_PARAMS;
	conf->hash_function = java_32bit_hash;
	return conf;
}


int set_language(hacf_conf_t *conf, const char *lang) {
	if(lang == NULL) {
        return NO_LANGUAGE;
	}
    if (!strcasecmp(lang, "java")) {
    	conf->language = JAVA_LANGUAGE;
    	conf->hash_function = java_32bit_hash;
        return JAVA_LANGUAGE;
    } else if (!strcasecmp(lang, "php")) {
    	conf->language = PHP_LANGUAGE;
    	conf->hash_function = php_32bit_hash;
        return PHP_LANGUAGE;
    } else if (!strcasecmp(lang, "python")) {
    	conf->language = PYTHON_LANGUAGE;
    	conf->hash_function = python_32bit_hash;
        return PYTHON_LANGUAGE;
    } else {
    	conf->language = NO_LANGUAGE;
    	conf->hash_function = java_32bit_hash;
        return NO_LANGUAGE;
    }
}

int get_language(const char *lang) {
    if (!strcasecmp(lang, "java")) {
        return JAVA_LANGUAGE;
    } else if (!strcasecmp(lang, "php")) {
        return PHP_LANGUAGE;
    } else if (!strcasecmp(lang, "python")) {
        return PYTHON_LANGUAGE;
    } else {
        return NO_LANGUAGE;
    }
}
