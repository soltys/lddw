#pragma once

#undef ERROR

/* https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation */
#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH_A 260
#define MAX_PATH_W 32767

#ifdef UNICODE
#define MAX_PATH MAX_PATH_W
#else
#define MAX_PATH MAX_PATH_A
#endif

#define malloc(size) HeapAlloc(GetProcessHeap(), 0, size)
#define free(ptr) HeapFree(GetProcessHeap(), 0, ptr)
#define realloc(ptr, size) HeapReAlloc(GetProcessHeap(), 0, ptr, size)
