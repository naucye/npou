//
// Created by naucye on 2022/8/15.
//

#ifndef NPOU_NPOU_H
#define NPOU_NPOU_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "http_download.h"
#include "npu_db.h"
#include "npu.h"
#include "string_util.h"
#include "file_util.h"

int execmd(char* cmd,char* result);

int npu_pkg_install(char *npu);

int npu_pkg_get(char *arch, char *npu);

int npu_db_query(struct Npu_DB *npu_db, char *package);

#endif //NPOU_NPOU_H
