#include "npou.h"

int main(int argc, char* argv[]) {
    char arch[10];
    execmd("uname -m",arch);
    arch[7] = '\0';
    struct Npu_DB npu_db[7];
    int npu_db_result;
    char **depend = {0};
	for (int i = 1; i < argc; i++) {
		if(strcmp("-S", argv[i]) == 0){
		    i++;
            printf("Query database... ");
            for (int k = 0; k + i < argc; k++) {
                if (npu_db_query(npu_db, argv[i + k])) {
                    printf("no");
                    printf("\nError: Not found package in '%s'\n", argv[i + k]);
                    exit(0);
                } else {
                    printf(" ok\n");
                }
            }
            printf("Resolve dependencies...");
            int depend_length = 0;
            depend = str_split(npu_db->depend, "  ", &depend_length);
            printf("ok\n");

            printf("\n");

            printf("Package (%d):  ",  argc - i + depend_length);
		    for (int k = 0; k + i < argc; k++) {
		        printf("%s  ", argv[i + k]);
		    }

            for (int k = 0; k < depend_length; ++k) {
                printf("%s  ", depend[k]);
            }

            printf("\n\n");

            printf("Query depend for database...\n");
            for (int k = 0; k < depend_length; ++k) {
                printf("Query %s... ", depend[k]);
                if (npu_db_query(npu_db, depend[k]) == 0){
                    printf(" ok\n");
                } else {
                    printf("no");
                    printf("\nError: Not found '%s' package\n", depend[k]);
                    exit(0);
                }
            }

		    printf("\n");
		    for (int k = 0; k + i < argc; k++) {
		        npu_pkg_get(arch, argv[i + k]);
		    }

            for (int k = 0; k < depend_length; ++k) {
                npu_pkg_get(arch, depend[k]);
            }

            for (int k = 0; k + i < argc; k++) {
                printf("Install %s ...", argv[i +k]);
                if(npu_pkg_install(argv[i + k]) == 0){
                    printf("ok\n");
                } else {
                    printf("on\n");
                }
            }

            for (int k = 0; k < depend_length; k++) {
                printf("Install %s ...", depend[k]);
                if(npu_pkg_install(depend[k]) == 0){
                    printf("ok\n");
                } else {
                    printf("on\n");
                }
            }
            printf("Processing package...\n");
            printf("Install Success!");
		} else if (strcmp("-Ud", argv[i]) == 0){
            npu_update_db(arch);
		}
	}
	return 0;
}

int execmd(char *cmd, char *result) {
	FILE *pipe = popen(cmd, "r");
	if(!pipe)
		return 0;
	
	char buffer[1024] = {0};
	while(!feof(pipe)) {
		if(fgets(buffer, 1024, pipe))
			strcat(result, buffer);
	}
	pclose(pipe);
	return 1;
}

int npu_pkg_install(char *npu){
    char path[256] = {0};
    char c[1024] = {0};

    strcat(path, "/var/npou/cache/");
    strcat(path, npu);
    strcat(path, ".npu");

    strcpy(c, "mkdir -vp /var/npou/cache /var/npou/packages /var/npou/db /usr/share/npou /etc/npou");
    system(c);

    struct Npu npus[5];
    npu_getInfo(npus, path);

    strcpy(c, "mkdir -p /var/npou/packages/");
    strcat(c, npus->name);
    system(c);

    npu_unpack(npus, path, "/var/npou/packages");

    char pkg_script[256] = {0};
    strcat(pkg_script, "/var/npou/packages/");
    strcat(pkg_script, npus->name);
    strcat(pkg_script, "/MAKEPKG");

    printf("\n  => Running install script...");
    char script[256] = {0};
    sprintf(c, "sudo sh '%s'", pkg_script);
    system(c);
// BUG 获取数据乱码
//    FILE *pkg_script_file = fopen(pkg_script, "r");
//    if (pkg_script_file != NULL){
//        int pkg_script_size = read_bin_size(pkg_script_file);
//        char pkg_script_data[pkg_script_size];
//        strcpy(pkg_script_data, read_content(pkg_script_file, pkg_script_size));
//        printf("%s", pkg_script_data);
//        if (pkg_script_data != NULL && strcmp(pkg_script_data, "")) {
//            printf("\n  => Running install script...");
//            sprintf(c, "sudo sh '%s'", pkg_script);
//            system(c);
//        }
//    }

    sprintf(c, "NPU=%s ; tar -zxf /var/npou/packages/${NPU}.tar.gz -C /var/npou/packages/${NPU}", npus->name);
    system(c);
    sprintf(c, "NPU=%s ; for file in /var/npou/packages/${NPU}/data/* ; do  [ -d \"$file\" ] &&  if [ \"$(ls -A $file)\" ];then cp -rpvu $file/* /${file##*/} ; else mkdir -p /${file##*/}; fi ; done", npus->name);
    system(c);
    return 0;
}

int npu_pkg_get(char *arch, char *npu){
    char path[256] = {0};
    char c[1024] = {0};

    char *tmp = NULL;
    if ((tmp = strstr(arch, "\n"))){
        *tmp = '\0';
    }

    FILE *fp=fopen("/etc/npou/mirror", "r");
    char mirror_cmd[256] = {0};
    char mirror[256] = {0};
    char mirror_data[1024] = {0};
    ////////////////////--BUG
    fgets(mirror_data, 1024, fp);
    sprintf(mirror_cmd, "ARCH=%s;PACKAGE=%s.npu;echo -n %s", arch, npu, mirror_data);
    execmd(mirror_cmd, mirror);
    strcpy(c, "mkdir -vp /var/npou/cache /var/npou/packages /usr/share/npou /etc/npou");
    system(c);

    strcat(path, "/var/npou/cache");
    strcat(path, "/");
    strcat(path, npu);
    strcat(path, ".npu");
    http_get(mirror, path);
}

int npu_update_db(char *arch) {
    printf("Synchronizing database...\n");
    char *tmp = NULL;
    if ((tmp = strstr(arch, "\n"))){
        *tmp = '\0';
    }

    FILE *fp=fopen("/etc/npou/mirror", "rb+");
    char mirror_cmd[256] = {0};
    char mirror[256] = {0};
    char c[1024] = {0};
    sprintf(mirror_cmd, "ARCH=%s;PACKAGE=%s;echo -n %s", arch, "core.db", read_content(fp, read_bin_size(fp)));
    execmd(mirror_cmd, mirror);
    strcpy(c, "mkdir -p /var/npou/db");
    system(c);
    http_get(mirror, "/var/npou/db/core.db");
    printf("Success!");
}

int npu_pkg_remove(char *arch, char *npu){

}

int npu_db_query(struct Npu_DB *npu_db, char *package){
    int npu_db_result;
    npu_db_result = npu_db_read(npu_db, "/var/npou/db/core.db", package);
    if (npu_db_result != 0)
        npu_db_result = npu_db_read(npu_db, "/var/npou/db/community.db", package);
    if (npu_db_result != 0){
        return -1;
    }
    return 0;
}
 

