#pragma once

#define APP_NAME "lddw"

#include <windows.h>

void reqbr(void);

void pmsg(const LPCSTR msg);
void pmsgw(const LPCWSTR msg);

void cperr(const LPCSTR fname, const LPCSTR msg);
void cperre(const LPCSTR fname, const DWORD err);
void cperrle(const LPCSTR fname);

void cwperr(const LPCWSTR fname, const LPCWSTR msg);
void cwperre(const LPCWSTR fname, const DWORD err);
void cwperrle(const LPCWSTR fname);

void pu32x(unsigned int u32);
void pu64x(unsigned long long u64);

#ifdef _WIN64
#define pld64x pu64x
#else
#define pld64x pu32x
#endif
