#include "ldd_args.h"

#include "log.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define ARGBASE_MAX INT32_MAX

static bool wcs_starts_with(const wchar_t *str, const wchar_t *prefix) {
    size_t len = wcslen(prefix);
    return wcsncmp(str, prefix, len) == 0;
}

parse_args_code ldd_parse_args(int argc, LPWSTR *argv, ldd_args *args, int *argbase) {
    if (argv == NULL) {
        pmsg("lddw: Failed to parse command line\n");
        *argbase = ARGBASE_MAX;
        return FAILURE;

    } else if (argc < 2) {
        *argbase = ARGBASE_MAX;
        return MISSING_ARGS;
    }

    bool end_parsing = false;
    while (!end_parsing) {
        end_parsing = true;

        if (wcscmp(L"-h", argv[*argbase]) == 0 || wcscmp(L"--help", argv[*argbase]) == 0) {
            *argbase = ARGBASE_MAX;
            return PRINT_HELP;
        }

        if (wcscmp(L"-V", argv[*argbase]) == 0 || wcscmp(L"--version", argv[*argbase]) == 0) {
            *argbase = ARGBASE_MAX;
            return PRINT_VERSION;
        }

        if (wcscmp(L"-a", argv[*argbase]) == 0 || wcscmp(L"--all", argv[*argbase]) == 0) {
            args->view_all = true;
            end_parsing = false;
            (*argbase)++;
        }

        if (wcscmp(L"--format=cmake", argv[*argbase]) == 0) {
            args->format = FORMAT_CMAKE;
            end_parsing = false;
            (*argbase)++;
        }

        if (wcs_starts_with(argv[*argbase], L"--contains=")) {
            LPWSTR value = argv[*argbase] + wcslen(L"--contains=");
            args->contains = _wcsdup(value);
            end_parsing = false;
            (*argbase)++;
        }
    }
    return argc > *argbase ? DO_LDD : MISSING_ARGS;
}
