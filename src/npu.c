/**
 * Npu 包管理工具
 * Created by Naucye on 2022/06/05
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "string_util.h"
#include "file_util.h"
#include "config_file_read.h"
#include "npu.h"

/**
 * Npu 数据文件打包工具
 * @param datapath 数据包文件
 * @param destpath 生成的tar.gz目标文件
 * @return int
 */
int npu_package(char *datapath, char *destpath){
    printf("Build data package...\n");
    char c[512] = {0};
    strcat(c, "datapath=");
    // 数据文件打包为tar.gz文件命令
    if(destpath[0] == '/'){
        strcat(c, datapath);
    } else {
        strcat(c, "`pwd`/");
        strcat(c, datapath);
    }
    strcat(c, " ; ");
    strcat(c, "destpath=");
    strcat(c, destpath);
    strcat(c, " ; ");
    strcat(c, "rm -rf $datapath/.build ; [ ! -d \"$datapath/.build/package\" ] && mkdir -p $datapath/.build/package ; cd $datapath/.build/package ; cp -r $datapath/data . ; cp -r $datapath/source . ; cp -r $datapath/PKGINFO . ; cp -r $datapath/MAKEPKG . ; chmod -R 777 $datapath/.build/package/data/bin ; tar -zcvf $destpath .");
    system(c);
    printf("Package success\n");
    printf("Data output file is '%s'\n", destpath);
    return 0;
}

/**
 * Npu 编译打包工具
 * @param npu Npu结构体
 * @param datapath tar.gz类型的数据文件
 * @param destpath Npu目标文件
 * @return int
 */
int npu_build(struct Npu npu[5], char *datapath, char *destpath){
    printf("Build npu package...\n");

    int size = sizeof(struct Npu);

    FILE *fp=fopen(destpath, "wrb+");

    if(fp != NULL) {
        // 写入头文件
        printf("Write top file...\n");
        fwrite(NPU_TOP, 8, 1, fp);
        printf("Write top file size: %d Bit\n", 8);
        // 写入Npu文件信息
        printf("Top information:\n");
        printf("    Name: %s\n    Developer: %s\n    Version: %s\n    Arch: %s\n    Describe: %s\n",npu->name,npu->developer,npu->version, npu->arch, npu->describe);
        printf("\n");
        printf("Write top information...\n");
        fwrite(npu, size, 1, fp);
        printf("Write top information size: %d Bit\n", size);

        FILE *fpd=fopen(datapath,"rb+");
        if(fpd != NULL) {
            printf("Write data...\n");
            int sizes=read_bin_size(fopen(datapath,"rb+"));
            unsigned char data[sizes];
            read_bin(fpd, data,sizes);
            printf("Write data size: %d Bit\n", sizes);
//            printf("data content: ");
//            for(int i=0;i<sizes;i++){
//                if (i % 16 == 0)
//                    printf("\n");
//                printf("0x%02x,",data[i]);
//            }
//            printf("\n");

            fwrite(data ,sizes,1,fp);
            fclose(fpd);
        }

        fclose(fp);
    }
    printf("Build success\n");
    printf("Npu output file is '%s'\n", destpath);
    return 0;
}


int npu_getInfo(struct Npu npu[5], char *npupath){
    int size=sizeof(struct Npu);
    FILE *fp=fopen(npupath, "rb");
    if(fp != NULL){
        fseek( fp, 8, SEEK_SET);
        fread(npu, size, 1, fp);
        //printf("Name: %s\nDeveloper: %s\nVersion: %s\nArch: %s\nDescribe: %s\n",npu->name,npu->developer,npu->version, npu->arch, npu->describe);
        fclose(fp);
    }
    return 0;
}

int npu_unpack(struct Npu npu[5], char *npupath, char *datapath){
    char filename[256] = {0};
    strcat(filename, datapath);
    strcat(filename, "/");
    strcat(filename, npu->name);
    strcat(filename, ".tar.gz");

    FILE *fpd = fopen(filename, "wrb+");
    FILE *fpn=fopen(npupath, "rb+");

    u32 sizes=read_bin_size(fopen(npupath, "rb+"));
    unsigned char data[sizes];
    fseek(fpn, NPU_INFO_SIZR, SEEK_SET);
    read_bin(fpn, data, sizes);

    fwrite(data, sizes-NPU_INFO_SIZR, 1, fpd);
    fclose(fpd);
    fclose(fpn);
    //printf("Output file is '%s'\n", filename);
    return 0;
}



