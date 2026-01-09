#include <soltystest/soltystest.h>

#include <src/ldd_args.h>


static int test_no_args_added(const test_context* t) { 
    LPWSTR argv[] = {L"lddw.exe"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc,argv,&ldd_args,&argbase);

    if (parse_code != MISSING_ARGS) {
        return test_error(t, "parse args code is not FAILURE");
    }

    return 0;
}

static int test_output_is_set_to_cmake(const test_context *t) {
    LPWSTR argv[] = {L"lddw.exe", L"--output=cmake", L"my.dll"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    ldd_args ldd_args = {0};
    int argbase = 1;
    parse_args_code parse_code = ldd_parse_args(argc, argv, &ldd_args, &argbase);

    if (parse_code != DO_LDD) {
        return test_error(t, "parse_code is not DO_LDD");
    }

    if (ldd_args.output_format != OUTPUT_CMAKE) {
        return test_error(t, "output_format is not OUTPUT_CMAKE");
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

    if (ldd_args.output_format != OUTPUT_LIST) {
        return test_error(t, "output_format is not OUTPUT_LIST");
    }

    return 0;
}

static test_func_reg test_functions[] = {
    ADD_TEST(test_no_args_added),
    ADD_TEST(test_output_is_set_to_cmake),
    ADD_TEST(test_default_output_is_set_to_list)
};

int main(int argc, char **argv) { return run_test_suite("test_args", test_functions); }
