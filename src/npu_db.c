//
// Created by naucye on 2022/7/9.
//
#include "npu_db.h"
/**
 * Npu DB数据写入工具
 * @param db Npu_DB
 * @param source_path DB文件路径
 */
void npu_db_write(struct Npu_DB *db, char *source_path) {
    int size = sizeof(struct Npu_DB);
    printf("db size: %d\n", size);
    FILE *fp=fopen(source_path, "ab");
    if(fp != NULL) {
        long source_size = read_bin_size(fp);
        if (source_size == 0) {
            // 写入头文件
            fwrite("NPUDB", 8, 1, fp);
        }
        fwrite(db, size, 1, fp);
    }
    fclose(fp);
}
/**
 * Npu DB 读取工具
 * @param db Npu_DB
 * @param source_path DB文件路径
 * @param package 软件包，用于查询软件包是否存在
 * @return
 */
int npu_db_read(struct Npu_DB *db, char *source_path, char *package){//1M block 2946     ; 1 block 0.000340MB
    int size=sizeof(struct Npu_DB);
    FILE *fp=fopen(source_path, "rb");
    if(fp != NULL){
        long source_size = read_bin_size(fp);
        long block = source_size / NPU_DB_SIZE;
//        printf("block: %d\n", block);
        fseek( fp, 8, SEEK_SET);
//        if((float )block * size/1024/1024 > 1)
//            exit(0);
        for (int i = 0; i < block; ++i) {
            if (i > 1)
                fseek( fp, size, block * size + 8);

            fread(db, size, 1, fp);
            //printf("%d",strcmp(db->name, package) == -58);
            if (package != NULL && strcmp(db->name, package) == 0) {
                break;
            }
//            if(db != NULL)
//                printf("db %d: %s\n", i +1, db->name);

            if (package != NULL && block == i +1 && strcmp(db->name, package) != 0) {
                //printf("not package\n");
                return -1;
            }
        }
//        printf("total size: %fMB\n", (float )block * size/1024/1024);

        fclose(fp);
        return 0;
    }
    return -1;
}