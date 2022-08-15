//
// Created by naucye on 2022/7/9.
//
#include "npu_db.h"

void npu_help(){
    printf("Usage： \n");
    printf("npudb [Options]...\n\n");
    printf("Options： \n");
    printf("  -h,--help                                       Get npu help information\n");
    printf("  -f, --npu-config-file [npu_config_file]         For read insert\n");
    printf("  -d, --npudb-file [npudb_file]                   Npudb database object file\n");
    printf("  -p, --npu-file [npu_file]                       Npudb file\n");
}

int main(int argc, char* argv[]){
    char path[256] = {0};
    char pkg_path[256] = {0};
    char db_path[256] = "/var/npou/db/core.db";
    for (int i = 1; i < argc; i++){
        if(equals(argv[i], "-h") || equals(argv[i], "--help")) {
            npu_help();
            exit(0);
        } else if(equals(argv[i], "-f")) {
            i++;
            strcpy(path, argv[i]);
        } else if(equals(argv[i], "-d")) {
            i++;
            strcpy(db_path, argv[i]);
        } else if(equals(argv[i], "-p")) {
            i++;
            strcpy(pkg_path, argv[i]);
        }
    }

    struct Npu_DB db[7];
    char **fileData = NULL;
    int lines = 0;
    struct ConfigInfo *info = NULL;
    FILE *fp = fopen(pkg_path, "rb+");
    u32 size_u32 = read_bin_size(fp);
    char size[1024];
    sprintf(size, "%d",size_u32);

    //加载配置文件
    loadFile_ConfigFile(path, &fileData, &lines);
    //解析配置文件
    parseFile_ConfigFile(fileData, lines, &info);

    strcpy(db->name, getInfo_ConfigFile("name",info,lines));
    strcpy(db->developer, getInfo_ConfigFile("developer",info,lines));
    strcpy(db->version, getInfo_ConfigFile("version",info,lines));
    strcpy(db->arch, getInfo_ConfigFile("arch",info,lines));
    strcpy(db->describe, getInfo_ConfigFile("describe",info,lines));
    strcpy(db->memory, size);
    strcpy(db->depend, getInfo_ConfigFile("depend",info,lines));
    //释放配置信息内存
    destroInfo_ConfigFile(info);
    npu_db_write(db, db_path);

    printf("Name: %s\nDeveloper: %s\nVersion: %s\nArch: %s\nDescribe: %s\nMemory: %s\nDepend: %s\n",db->name,db->developer,db->version, db->arch, db->describe, db->memory, db->depend);
    printf("Written to database, size is %d bit\n", size);
}
