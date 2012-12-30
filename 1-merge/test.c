#include <check.h>
#include <stdlib.h>

#include "merge.h"

TYPE *merge_ref(const TYPE *a, int n, const TYPE *b, int m)
{
    TYPE *c = calloc(n + m, sizeof(TYPE));
    if (c == NULL) {
        return NULL;
    }

    int i = 0;
    int j = 0;
    int k = 0;

    while (i < n && j < m) {
        c[k++] = ((a[i] < b[j]) ? a[i++] : b[j++]);
    }

    while (i < n) c[k++] = a[i++]; 
    while (j < m) c[k++] = b[j++]; 

    return c;
}

START_TEST(test_singles)
{
    TYPE a[] = { 0 };
    TYPE b[] = { 1 };

    size_t n = sizeof(a) / sizeof(a[0]);
    size_t m = sizeof(b) / sizeof(b[0]);

    TYPE *ref = merge_ref(a, n, b, m);
    TYPE *tst = merge(a, n, b, m);

    fail_unless(memcmp(ref, tst, sizeof(TYPE) * (n + m)) == 0,
            "Result not equal to reference implementation");

    free(ref);
    free(tst);
}
END_TEST

static Suite *create_suite(void)
{
    Suite *s = suite_create("merge");
    TCase *tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_singles);

    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc __attribute__ ((unused)), char **argv __attribute__ ((unused)))
{
    int number_failed;
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
