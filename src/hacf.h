/*
 * Name        : hacf.h
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
#ifndef HACF_H
#define HACF_H 1
#include "httpd.h"
#include "hacf_param.h"

#define DEFAULT_LANGUAGE NO_LANGUAGE
#define DEFAULT_MAX_COLLISIONS 3
#define DEFAULT_MAX_PARAMS 256

#define NO_LANGUAGE 0
#define JAVA_LANGUAGE 1
#define PYTHON_LANGUAGE 2
#define PHP_LANGUAGE 3

#define ASSERT_OK	DECLINED
#define ASSERT_BAD	HTTP_BAD_REQUEST

/**
 * Module configuration, can be used at server or directory level.
 */
typedef struct hacf_conf_t hacf_conf_t;
struct hacf_conf_t {
	/** language protection needed */
	int language;
	/** maximum number of allowed collisions */
	int max_collisions;
	/** maximum number of allowed parameters */
	int max_parameters;
	/** hash calculation function */
	uint32_t (*hash_function)(const char *);
};

/**
 * Assertion result.
 */
typedef struct hacf_assert_t hacf_assert_t;
struct hacf_assert_t {
    /**
     *  Assert result.
     *  Possible values :
     *   - ASSERT_BAD, a specified limit was broken, Apache should stop further processing.
     *   - ASSERT_OK everything went fine, Apache should continue processing
     */
	int result;
	 /** unparsed content, should be prepended to the next bucket */
    const char *remains;
    /** text message describing the result */
    const char *message;
    /** parameter list */
    hacf_param_list_t *list;
};

/**
 * Create configuration structure
 */
extern hacf_conf_t *create_hacf_conf(apr_pool_t *pool);

/**
 * Return chosen language, NO_LANGUAGE when no match was found
 */
extern int get_language(const char *lang);

/**
 * Set the language parameters on hacf_conf_t, return chosen language, NO_LANGUAGE when no match was found
 */
extern int set_language(hacf_conf_t *conf, const char *lang);

/**
 * Parse the parameters, check against the assertion parameters and return assertion result
 *
 */
extern hacf_assert_t *assert_parameters(hacf_assert_t *assert, const char *query, const char *prefix, hacf_conf_t *conf);
extern char *get_next_parameter(apr_pool_t *pool, const char *start, apr_size_t *offset);
extern uint32_t java_32bit_hash(const char *key);
extern uint32_t php_32bit_hash(const char *key);
extern uint32_t python_32bit_hash(const char *key);
extern hacf_assert_t *create_hacf_assert(apr_pool_t *pool);

#endif
