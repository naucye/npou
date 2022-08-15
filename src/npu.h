//
// Created by naucye on 2022/8/15.
//

#ifndef NPOU_NPU_H
#define NPOU_NPU_H


const char NPU_TOP[8] = "NPU";

const int NPU_INFO_SIZR = 112;

struct Npu{
    char name[12];
    char developer[10];
    char version[10];
    char arch[8];
    char describe[64];
};

char * strcopy(char *str1, char * str2);

void npu_help();

int npu_package(char *datapath, char *destpath);

int npu_build(struct Npu npu[5], char *datapath, char *destpath);

int npu_getInfo(struct Npu npu[5], char *npupath);

int npu_unpack(struct Npu npu[5], char *datapath, char *destpath);


#endif //NPOU_NPU_H
