#include "ldd.h"
#include "ldd_args.h"
#include "log.h"

#define RES_MISSING_FILE_ARGS "lddw: missing file arguments\n"
#define RES_HELP_MSG "Try `lddw --help' for more information.\n"
#define RES_HELP_MANUAL                                                                                                \
    "Usage: lddw [OPTION]... FILE...\n"                                                                                \
    "\n"                                                                                                               \
    "Print shared library dependencies\n"                                                                              \
    "\n"                                                                                                               \
    "  -a, --all               print all dependencies including API-set\n"                                             \
    "  -f, --flatten           print dependencies on flatten format, not tree "                                        \
    "format\n"                                                                                                         \
    "  -h, --help              print this help and exit\n"                                                             \
    "  -V, --version           print version information and exit\n"

#define RES_VERSION                                                                                                    \
    "lddw (Windows NT) 0.1\n"                                                                                          \
    "This is free software; see the source for copying conditions.\n"

int main(int _, char **__) {
    int argc = -1;
    int argbase = 1;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    ldd_args args = {0};

    parse_args_code parse_code = ldd_parse_args(argc, argv, &args, &argbase);
    if (parse_code == MISSING_ARGS) {
        pmsg(RES_MISSING_FILE_ARGS);
        pmsg(RES_HELP_MSG);
        return EXIT_FAILURE;
    } else if (parse_code == PRINT_HELP) {
        pmsg(RES_HELP_MANUAL);
        return EXIT_SUCCESS;
    } else if (parse_code == PRINT_VERSION) {
        pmsg(RES_VERSION);
        return EXIT_SUCCESS;

    } else if (parse_code == DO_LDD) {
        for (int i = argbase; i < argc; ++i) {
            if (ldd(args, argv[i]) < 0) {
                cwperr(argv[i], L"Failed to load dependencies");
            }
        }
    }

    if (argv) {
        LocalFree((void *)argv);
    }

    return EXIT_SUCCESS;
}
