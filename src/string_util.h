//
// Created by naucye on 2022/8/15.
//

#ifndef NPOU_STRING_UTIL_H
#define NPOU_STRING_UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_util.h"

char * strcopy(char *str1, char * str2);

int equals(const char *str1, const char *str2);

char** str_split(char *str, char *delimiter, int *length);

#endif //NPOU_STRING_UTIL_H
