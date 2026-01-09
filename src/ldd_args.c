#include "ldd_args.h"

#include "log.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define ARGBASE_MAX INT32_MAX

parse_args_code ldd_parse_args(int argc, LPWSTR *argv, ldd_args *args, int *argbase) {
    if (argv == NULL) {
        pmsg("lddw: Failed to parse command line\n");
        *argbase = ARGBASE_MAX;
        return FAILURE;

    } else if (argc < 2) {     
        *argbase = ARGBASE_MAX;
        return MISSING_ARGS;
    } else if (wcscmp(L"-h", argv[*argbase]) == 0 || wcscmp(L"--help", argv[*argbase]) == 0) {        
        *argbase = ARGBASE_MAX;
        return PRINT_HELP;
    } else if (wcscmp(L"-V", argv[*argbase]) == 0 || wcscmp(L"--version", argv[*argbase]) == 0) {        
        *argbase = ARGBASE_MAX;
        return PRINT_VERSION;
    } 

    if (wcscmp(L"-a", argv[*argbase]) == 0 || wcscmp(L"--all", argv[*argbase]) == 0) {
        args->bViewAll = true;
        (*argbase)++;
    }

    if (wcscmp(L"--output=cmake", argv[*argbase]) == 0) {
        args->output_format = OUTPUT_CMAKE;
        (*argbase)++;
    }


    return DO_LDD;
}