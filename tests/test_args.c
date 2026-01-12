#include <soltystest/soltystest.h>

#include <src/ldd_args.h>

static int test_no_args_added(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != MISSING_ARGS) {
        return test_error(t, "parse args code is not FAILURE");
    }

    return 0;
}

static int test_output_is_set_to_cmake(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe", L"--format=cmake", L"my.dll"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != DO_LDD) {
        return test_error(t, "parse_code is not DO_LDD");
    }

    if (ldd_args.format != FORMAT_CMAKE) {
        return test_error(t, "output_format is not FORMAT_CMAKE");
    }

    return 0;
}

static int test_output_is_set_but_no_positional_args(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe", L"--format=cmake"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != MISSING_ARGS) {
        return test_error(t, "parse_code is not MISSING_ARGS");
    }

    return 0;
}

static int test_default_output_is_set_to_list(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe", L"my.dll"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != DO_LDD) {
        return test_error(t, "parse_code is not DO_LDD");
    }

    if (ldd_args.format != FORMAT_LIST) {
        return test_error(t, "output_format is not FORMAT_LIST");
    }

    return 0;
}

static int test_contains_is_set_to_value_given(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe", L"--contains=gtk", L"my.dll"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != DO_LDD) {
        return test_error(t, "parse_code is not DO_LDD");
    }

    if (ldd_args.contains == NULL) {
        return test_error(t, "ldd_args.contains is NULL");
    }

    if (wcscmp(L"gtk", ldd_args.contains) != 0) {
        return test_error(t, "ldd_args.contains is not gtk");
    }

    return 0;
}

static test_func_reg test_functions[] = {
    ADD_TEST(test_no_args_added), ADD_TEST(test_output_is_set_to_cmake), ADD_TEST(test_default_output_is_set_to_list),
    ADD_TEST(test_output_is_set_but_no_positional_args), ADD_TEST(test_contains_is_set_to_value_given)};

int main(int argc, char **argv) { return run_test_suite("test_args", test_functions); }
