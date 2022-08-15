//
// Created by naucye on 2022/7/9.
//

#ifndef NPT_NPU_DB_H
#define NPT_NPU_DB_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_util.h"
#include "config_file_read.h"

struct Npu_DB{
    char name[12];
    char developer[10];
    char version[10];
    char arch[8];
    char describe[64];
    char memory[12];
    char depend[240];
};

const int NPU_DB_SIZE = 356;

void npu_db_write(struct Npu_DB *db, char *source_path);

int npu_db_read(struct Npu_DB *db, char *source_path, char *package);

#endif //NPT_NPU_DB_H
