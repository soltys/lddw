#include "ldd.h"
#include "dict.h"
#include "fw.h"
#include "log.h"

#include <Psapi.h>
#include <tchar.h>
#include <windows.h>

#define OpenFileR(f) CreateFileW(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)

static int findlib(const LPCWSTR fname, const LPWSTR fpath, DWORD size);
static DWORD rva2offset(DWORD rva, const PIMAGE_SECTION_HEADER secH, const PIMAGE_NT_HEADERS ntH);
static int impl_static_ldd(const ldd_args args, LPCVOID fdat, LPCWSTR fname, int depth, struct DICT *dict);
static int impl_ldd(const ldd_args args, LPCWSTR fname, int depth, struct DICT *dict);

int ldd(const ldd_args args, const LPCWSTR fname) {
    struct DICT dict = dict__new(__eqcmp_wcs);

    int ret = impl_ldd(args, fname, 0, &dict);

    dict__free(&dict);

    return ret;
}

static int impl_ldd(const ldd_args args, const LPCWSTR fname, int depth, struct DICT *dict) {
    LPWSTR fpath = malloc(MAX_PATH_W * sizeof(WCHAR));
    fpath[0] = 0;
    if (GetFullPathNameW(fname, MAX_PATH_W, fpath, NULL) == 0) {
        cwperr(fname, L"Failed to get full path");
        return -1;
    }

    int ret = 0;
    HANDLE fp = OpenFileR(fpath);
    free(fpath);
    if (fp == NULL) {
        cwperr(fname, L"Failed to open file");
        cwperrle(fname);
        ret = -1;
        goto FINALIZE_FP;
    }
    HANDLE fmap = CreateFileMappingW(fp, NULL, PAGE_READONLY, 0, 0, NULL);
    if (fmap == NULL) {
        cwperr(fname, L"Failed to create file mapping");
        cwperrle(fname);
        ret = -1;
        goto FINALIZE_FMAP;
    }
    HANDLE fbase = MapViewOfFile(fmap, FILE_MAP_READ, 0, 0, 0);
    if (fbase == NULL) {
        cwperr(fname, L"Failed to view file mapping");
        cwperrle(fname);
        ret = -1;
        goto FINALIZE_FBASE;
    }

    ret = impl_static_ldd(args, fbase, fname, depth, dict);

FINALIZE_FBASE:
    if (fbase)
        UnmapViewOfFile(fbase);
FINALIZE_FMAP:
    if (fmap)
        CloseHandle(fmap);
FINALIZE_FP:
    if (fp)
        CloseHandle(fp);
    return ret;
}

static int impl_static_ldd(ldd_args args, const LPCVOID fdat, const LPCWSTR fname, int depth, struct DICT *dict) {
    PIMAGE_DOS_HEADER dosH = (PIMAGE_DOS_HEADER)fdat;
    PIMAGE_NT_HEADERS ntH = (PIMAGE_NT_HEADERS)((ULONG_PTR)fdat + dosH->e_lfanew);
    PIMAGE_SECTION_HEADER secH = IMAGE_FIRST_SECTION(ntH);
    DWORD impRVA;
    PIMAGE_IMPORT_DESCRIPTOR impDesc;

    // filter non-DOS binary
    if (dosH->e_magic != IMAGE_DOS_SIGNATURE) {
        if (depth == 0)
            cwperr(fname, L"DOS signature (MZ) not matched");
        return -1;
    }
    // filter non-NT binary
    if (ntH->Signature != IMAGE_NT_SIGNATURE) {
        if (depth == 0)
            cwperr(fname, L"PE signature (PE) not matched");
        return -1;
    }

    // filter independent PE file
    if (ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0) {
        if (depth == 0)
            cwperr(fname, L"Dependency not found");
        return -1;
    }

    impRVA = ntH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    impDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)fdat + rva2offset(impRVA, secH, ntH));
    for (; impDesc->Name != 0; ++impDesc) {
        // convert name from UTF-8 to UTF-16
        WCHAR namew[MAX_PATH_W];
        namew[0] = 0;
        {
            LPCSTR namea = (LPCSTR)((ULONG_PTR)fdat + rva2offset(impDesc->Name, secH, ntH));

            // determine if library is Windows API sets
            if ((strncmp("ext-", namea, 4) == 0 || strncmp("api-", namea, 4) == 0) && !args.bViewAll)
                continue;

            MultiByteToWideChar(CP_UTF8, 0, namea, strlen(namea) + 1, namew, MAX_PATH_W);
        }

        if (!dict__add(dict, namew, (wcslen(namew) + 1) * sizeof(WCHAR)))
            continue;

        // print library path
        LPWSTR fpath = malloc(MAX_PATH_W * sizeof(WCHAR));
        fpath[0] = 0;

        int found = findlib(namew, fpath, MAX_PATH_W);

        if (args.output_format == OUTPUT_LIST) {
            pmsgw(namew);
            pmsg(" => ");
            pmsgw((found == 0) ? fpath : L"NotFound");
            pmsg("\n");
        } else if (args.output_format == OUTPUT_CMAKE) {
            pmsgw(fpath);
            pmsg(";");
        }

        // recurse listing
        if (found == 0) {
            impl_ldd(args, fpath, depth + 1, dict);
        }

        free(fpath);
    }

    return 0;
}

static int findlib(const LPCWSTR fname, const LPWSTR fpath, DWORD size) {
    HMODULE lib = LoadLibraryExW(fname, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (lib == NULL) {
        // cwperr(fname, L"Failed to load module");
        // cwperrle(fname);
        return -1;
    }

    LPWSTR buf = malloc(MAX_PATH_W * sizeof(WCHAR));
    buf[0] = 0;
    DWORD ret = GetMappedFileNameW(GetCurrentProcess(), lib, buf, MAX_PATH_W);
    if (ret == 0) {
        // cwperr(fname, L"Failed to get module file name");
        // cwperrle(fname);
        FreeLibrary(lib);
        free(buf);
        return -1;
    }

    LPWSTR tmp = malloc(512 * sizeof(WCHAR));
    tmp[0] = 0;
    if (GetLogicalDriveStringsW(511, tmp) == 0) {
        // cwperr(fname, L"Failed to get logical drive name");
        // cwperrle(fname);
        FreeLibrary(lib);
        free(buf);
        free(tmp);
        return -1;
    }

    WCHAR driv[3];
    PWCHAR p = tmp;
    BOOL found = 0;

    driv[0] = L' ';
    driv[1] = L':';
    driv[2] = 0;

    do {
        WCHAR name[MAX_PATH_W];
        name[0] = 0;

        driv[0] = p[0];
        if (!QueryDosDeviceW(driv, name, MAX_PATH_W)) {
            continue;
        }

        size_t namelen = wcslen(name);
        if (namelen < MAX_PATH_W) {
            found = _wcsnicmp(buf, name, namelen) == 0 && *(buf + namelen) == L'\\';
            if (found) {
                WCHAR tmpfile[MAX_PATH_W];
                tmpfile[0] = 0;
                wcscat_s(tmpfile, MAX_PATH_W, driv);
                wcscat_s(tmpfile, MAX_PATH_W, (PWCHAR)(buf + namelen));
                wcscpy_s(buf, MAX_PATH_W, tmpfile);
            }
        }

        while (*p++)
            ;
    } while (!found && *p);

    wcscpy_s(fpath, size, buf);
    FreeLibrary(lib);
    free(buf);
    free(tmp);
    return 0;
}

static DWORD rva2offset(DWORD rva, const PIMAGE_SECTION_HEADER secH, const PIMAGE_NT_HEADERS ntH) {
    PIMAGE_SECTION_HEADER cpySecH = secH;
    if (rva == 0)
        return rva;
    for (size_t i = 0; i < ntH->FileHeader.NumberOfSections; ++i) {
        if (rva >= cpySecH->VirtualAddress && rva < cpySecH->VirtualAddress + cpySecH->Misc.VirtualSize)
            break;
        cpySecH++;
    }
    return rva - cpySecH->VirtualAddress + cpySecH->PointerToRawData;
}
