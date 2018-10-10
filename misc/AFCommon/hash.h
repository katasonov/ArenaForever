#pragma once

#include <stdint.h>
#include <stdio.h>
#include <memory>

uint32_t hashfile(const wchar_t *filename);

uint32_t hashwstr(const wchar_t *filename);