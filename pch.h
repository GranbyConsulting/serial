
#ifndef PCH_H
#define PCH_H

#define  _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <conio.h>
#include <stdio.h>

using namespace std;

#include <string>
#include <vector>
#include <fstream>

#include "span.hpp"

#include "Printf.hpp"

typedef BYTE const *PCBYTE;

typedef vector<BYTE> bytes;

typedef vspan<BYTE>  bspan;

#define AfxIgnore(x) ((void) (x))

#endif
