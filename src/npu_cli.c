/**
 * Npu-cli
 * Created by Naucye on 2022/06/05
 */

#include <stdio.h>
#include <string.h>
#include "npu.h"
#include "config_file_read.h"
#include "string_util.h"

void npu_help();


void npu_help(){
    printf("Usage： \n");
    printf("npu [Options]...\n\n");
    printf("Options： \n");
    printf("  -h,--help                         Get npu help information\n");
    printf("  -f, --npu-file [npu_file]         npu command to use\n");
    printf("  -g, --get-info [string]           Get npu information, get all information by default\n");
    printf("  build [package_dir]               Compile the current project, use the -f option to specify the location, the default current location\n");
}

int main(int argc, char* argv[]) {
    char npu_file[256];
    char data_file[256];
    for (int i = 1; i < argc; i++){
        if(equals(argv[i], "-h") || equals(argv[i], "--help")) {
            npu_help();
            exit(0);
        } else if(equals(argv[i], "-f")) {
            i++;
            strcpy(npu_file, argv[i]);
        } else if(equals(argv[i], "-g")) {
            i++;
            struct Npu npu[5];
            npu_getInfo(npu, npu_file);
            if (argv[i] == NULL) {
                printf("Name: %s\nDeveloper: %s\nVersion: %s\nArch: %s\nDescribe: %s\n",npu->name,npu->developer,npu->version, npu->arch, npu->describe);
            }
            exit(0);
        } else if (equals(argv[i], "build")) {
            strcpy(data_file, argv[i +1]);
            struct Npu npu[5];
            char **fileData = NULL;
            int lines = 0;
            struct ConfigInfo *info = NULL;
            char pkginfo_file[256] = {0};
            strcat(pkginfo_file, data_file);
            strcat(pkginfo_file, "/PKGINFO");
            //加载配置文件
            loadFile_ConfigFile(pkginfo_file, &fileData, &lines);
            //解析配置文件
            parseFile_ConfigFile(fileData, lines, &info);

            strcpy(npu->name, getInfo_ConfigFile("name",info,lines));
            strcpy(npu->developer, getInfo_ConfigFile("developer",info,lines));
            strcpy(npu->version, getInfo_ConfigFile("version",info,lines));
            strcpy(npu->arch, getInfo_ConfigFile("arch",info,lines));
            strcpy(npu->describe, getInfo_ConfigFile("describe",info,lines));
            //释放配置信息内存
            destroInfo_ConfigFile(info);
            char tar_file[256];

            sprintf(tar_file, "%s/.build/%s.tar.gz", data_file, npu->name);
            sprintf(npu_file, "%s/.build/%s.npu",data_file, npu->name);
            npu_package(data_file, tar_file);
            printf("\n");
            npu_build(npu, tar_file, npu_file);
            struct Npu na[5];
            printf("\n");
            //printf("Test package...\n");
            //printf("Get information\n");
            //npu_getInfo(na, npu_file, "/home/naucy/Projects/clang/npt/data/nshell/.build/test");
            //printf("Npu %s create success\n", na->name);
        }
    }
}
