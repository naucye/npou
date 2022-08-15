//
// Created by naucye on 2022/8/15.
//


#ifndef NPOU_FILE_UTIL_H
#define NPOU_FILE_UTIL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_util.h"

typedef unsigned char u8;
typedef unsigned long int u32;

u32 read_bin_size(FILE *fp);

void read_bin(FILE *infile, u8 *buf, u32 size);

char *read_content(FILE *fp, int size);

#endif //NPOU_FILE_UTIL_H
