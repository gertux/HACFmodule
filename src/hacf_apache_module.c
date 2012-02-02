/*
 * Name        : hacf_apache_module.c
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
#include "hacf.h"
#include "http_config.h"
#include "http_log.h"
#include "util_filter.h"

static const char *HACF_IN = "HACF_IN";
module AP_MODULE_DECLARE_DATA hacf_module;

static int assert_post(ap_filter_t *f, apr_bucket *b, hacf_conf_t *conf) {
	hacf_assert_t *assert = (hacf_assert_t *) ap_get_module_config(f->r->request_config, &hacf_module);

    if (!(APR_BUCKET_IS_METADATA(b))) {
        const char *buf;
        apr_size_t nbytes;
        char *obuf;
        if (apr_bucket_read(b, &buf, &nbytes, APR_BLOCK_READ) == APR_SUCCESS) {
            if (nbytes) {
                obuf = malloc(nbytes+1);
                memcpy(obuf, buf, nbytes);
                obuf[nbytes] = '\0';
#ifdef DEBUG
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, f->r, "hacf:  %s", obuf);
#endif /* DEBUG */
    			assert = assert_parameters(assert, obuf, assert->remains, conf);
                free(obuf);
    			if (assert->result == ASSERT_BAD) {
    				ap_log_rerror(APLOG_MARK,APLOG_ERR,0,f->r,
    						"Possible \"Hash Algorithm Collision\" Attack: %s ! Ignoring request.", assert->message);
    				return ASSERT_BAD;
    			}
            }
        } else {
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, f->r, "hacf:  error reading data from %s", b->type->name);
        }
    }
    return ASSERT_OK;
}

static apr_status_t hacf_input_filter(ap_filter_t *f, apr_bucket_brigade *bb,
	    ap_input_mode_t mode, apr_read_type_e block, apr_off_t readbytes) {

	/* don't care about these */
    if (mode != AP_MODE_READBYTES) {
        return ap_get_brigade(f->next, bb, mode, block, readbytes);
    }

    apr_status_t ret = APR_SUCCESS;
    hacf_conf_t *conf = (hacf_conf_t *) ap_get_module_config(f->r->per_dir_config, &hacf_module);

    apr_bucket *b;

    ret = ap_get_brigade(f->next, bb, mode, block, readbytes);

    if (ret == APR_SUCCESS) {
        for (b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b)) {
        	int assert_result = assert_post(f, b, conf);
#ifdef DEBUG
        	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, f->c->base_server, "HACF hacf_input_filter assert result: %d", assert_result) ;
#endif /* DEBUG */
        	if(assert_result != ASSERT_OK) {
        		return assert_result;
        	}
        }
    } else {
#ifdef DEBUG
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, f->c->base_server, "HACF hacf_input_filter no success getting brigade: %s - %d", f->frec->name, ret) ;
#endif /* DEBUG */
        return ret;
    }

    return APR_SUCCESS;
}

static int hacf_input_filter_setup(ap_filter_t *f) {
#ifdef DEBUG
	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, f->c->base_server, "HACF hacf_input_filter_setup");
#endif /* DEBUG */
	return OK;
}

static int hacf_translate_name(request_rec *request) {
    hacf_conf_t *conf = (hacf_conf_t *) ap_get_module_config(request->per_dir_config, &hacf_module);
#ifdef DEBUG
    if(conf) {
    	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: conf->language = %d", conf->language);
    } else {
    	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: conf = null");
    }
#endif /* DEBUG */
	if (conf->language == NO_LANGUAGE) {
		/* HACF module is not configured, bail out fast */
#ifdef DEBUG
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: Module disabled");
#endif /* DEBUG */
		return ASSERT_OK;
	}
	if(request->main != NULL) {
		/* subrequest, TODO handle */
#ifdef DEBUG
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: Subrequest, ignored for now !!!");
#endif /* DEBUG */
		return ASSERT_OK;
	}
	hacf_assert_t *assert = create_hacf_assert(request->pool);
	ap_set_module_config(request->request_config, &hacf_module, assert);
	if (request->args) {
		/* Handle request arguments */
		assert = assert_parameters(assert, request->args, NULL, conf);
#ifdef DEBUG
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: Args = %s", request->args);
		ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: Result = %d, %s", assert->result, assert->message);
#endif /* DEBUG */
	}
	if (request->method_number == M_POST) {
		const char *content_type = apr_table_get(request->headers_in, "Content-Type");
		if(strncmp("multipart/form-data", content_type, 19)) {
			/* Delegate POSTs to the input filter */
#ifdef DEBUG
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: post");
#endif /* DEBUG */
			ap_add_input_filter(HACF_IN, NULL, request, request->connection);
#ifdef DEBUG
		} else {
			/* skip multiparts for now, TODO catch these too */
	    	ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, request, "HACF hacf_translate_name: multipart, skipping");
#endif /* DEBUG */
		}
	}
	return assert->result;
}

static void mod_hacf_register_hooks(apr_pool_t *p) {
	ap_hook_translate_name(hacf_translate_name, NULL, NULL, APR_HOOK_REALLY_FIRST);
    ap_register_input_filter(HACF_IN, hacf_input_filter, hacf_input_filter_setup, AP_FTYPE_CONNECTION + 3);
}

static void *create_hacf_server_conf(apr_pool_t *pool, server_rec *svr) {
#ifdef DEBUG
	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, svr, "HACF create_hacf_server_conf");
#endif /* DEBUG */
	return create_hacf_conf(pool);
}

static void *create_hacf_dir_conf(apr_pool_t *pool, char *dir) {
	return create_hacf_conf(pool);
}

static const char *hacf_conf_language(cmd_parms* cmd, void *cfg, const char *arg) {
	hacf_conf_t *conf = (hacf_conf_t *)cfg;
	int language = set_language(conf, arg);
    return NULL;
}

static const command_rec hacf_commands[] = {
		AP_INIT_TAKE1("HacfLanguage", hacf_conf_language, NULL, RSRC_CONF | ACCESS_CONF,
				"language protection needed"),
		AP_INIT_TAKE1("HacfMaxCollisions", ap_set_int_slot, (void*)APR_OFFSETOF(hacf_conf_t,max_collisions), RSRC_CONF | ACCESS_CONF,
				"maximum number of allowed collisions"),
		AP_INIT_TAKE1("HacfMaxParameters", ap_set_int_slot, (void*)APR_OFFSETOF(hacf_conf_t,max_parameters), RSRC_CONF | ACCESS_CONF,
				"maximum number of allowed parameters"),
		{NULL}
};

module AP_MODULE_DECLARE_DATA hacf_module = {
		STANDARD20_MODULE_STUFF,
		create_hacf_dir_conf,
		NULL,
		create_hacf_server_conf,
		NULL,
		hacf_commands,
		mod_hacf_register_hooks
};
