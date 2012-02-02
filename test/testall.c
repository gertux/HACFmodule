/*
 * Name        : testall.c
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

/* Top-level pool which can be used by tests. */
apr_pool_t *p;

void apr_assert_success(CuTest* tc, const char* context, apr_status_t rv) {
	if (rv == APR_ENOTIMPL) {
		CuNotImpl(tc, context);
	}

	if (rv != APR_SUCCESS) {
		char buf[STRING_MAX], ebuf[128];
		sprintf(
				buf,
				"%s (%d): %s\n", context, rv, apr_strerror(rv, ebuf, sizeof ebuf));
		CuFail(tc, buf);
	}
}

static const struct testlist {
	const char *testname;
	CuSuite *(*func)(void);
} tests[] = {
		{ "testhacf", testhacf },
		{"testhacf_param", testhacf_param},
		{ "LastTest", NULL }
};

int main(int argc, char *argv[]) {
	CuSuiteList *alltests = NULL;
	CuString *output = CuStringNew();
	int i;
	int exclude = 0;
	int list_provided = 0;

	apr_initialize();
	atexit(apr_terminate);

	CuInit(argc, argv);

	apr_pool_create(&p, NULL);

	alltests = CuSuiteListNew("All Tests");
	for (i = 0; tests[i].func != NULL; i++) {
		CuSuiteListAdd(alltests, tests[i].func());
	}

	CuSuiteListRunWithSummary(alltests);
	i = CuSuiteListDetails(alltests, output);
	printf("%s\n", output->buffer);

	return i > 0 ? 1 : 0;
}

