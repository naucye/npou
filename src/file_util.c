//
// Created by naucye on 2022/7/9.
//

#include "file_util.h"

u32 read_bin_size(FILE *fp) {
    u32  size = 0;
    if (fp) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
    }
    return size;
}


void read_bin(FILE *infile, u8 *buf, u32 size) {
    fread(buf, sizeof(u8), size, infile);
}

char *read_content(FILE *fp, int size) {
    char *str;
    char txt[size];

    if (fp == NULL){
        return 0;
    }

    fseek(fp,0,SEEK_END);
    str=(char *)malloc(size);
    rewind(fp);
    while((fgets(txt,size,fp))!=NULL){
        strcat(str,txt);
    }
    fclose(fp);
    return str;
}

