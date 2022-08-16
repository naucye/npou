/**
 * Created by naucy on 7/6/22.
 **/

#include "string_util.h"

char * strcopy(char *str1, char * str2) {
    char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);
    char *c = result;

    if(result == NULL)
        return NULL;

    while(*str1 != '\0') {
        *result++ = *str1++;
    }
    while(*str2 != '\0') {
        *result++ = *str2++;
    }
    return c;
}

int equals(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}


char** str_split(char *str, char *delimiter, int *length) {
    int len = strlen(str);
    char *strCopy = (char*)malloc((len + 1) * sizeof(char)); //额外分配多一个字符空间来存储'\0'
    strcpy(strCopy, str); //复制str字符串
    //将strCopy中的每个分隔符赋值为'\0'
    for (int i = 0; strCopy[i] != '\0'; i++) {
        for (int j = 0; delimiter[j] != '\0'; j++) {
            if (strCopy[i] == delimiter[j]) {
                strCopy[i] = '\0';
                break;
            }
        }
    }
    //为字符串数组分配空间,额外分配多一个字符串指针并赋值为NULL来作为字符串结束标志
    char** res = (char**)malloc((len + 2) * sizeof(char*));
    len++; //遍历到strCopy最后的'\0'才结束
    int resI = 0; //每一个分隔符和原字符串的'\0'标志依次作为数组中的字符串的结束标志
    for (int i = 0; i < len; i++) {
        *length = resI +1;
        res[resI++] = strCopy + i;
        while (strCopy[i] != '\0') {
            i++;
        }
    }
    res[resI] = NULL; //字符串数组中结束标志
    return res;
}

/*
 截取字符串
@str 要被截取的字符串
@start 从什么位置开始截取
@end 到哪个位置结束截取
*/
char* strsub(char* str, int start, int end){

    char* result = (char*)malloc((end - start)*sizeof(char));
    str += start;
    for (int i = 0; i < (end-start); i++)
    {

        result[i] = *str;
        str++;

    }
    result[(end - start)] = '\0';
    return result;
}


char* strep(char* str,char* src,char* des){
    //返回的结果
    char* result = (char*)malloc((strlen(str) - strlen(src) + strlen(des))*sizeof(char));

    //先判断是否包被替换的字符串,返回的是要查找的字符串的首地址
    char* pos = strstr(str,src);
    if (!pos){
        //如果不包含直接返回
        return str;
    }
    //计算src的位置
    int start_end = pos - str;
    char* start = strsub(str, 0, start_end);

    int end_start = start_end + strlen(src);
    char* end = strsub(str, end_start, strlen(str));

    strcpy(result, start);
    strcat(result,des);
    strcat(result,end);

    return strep(result,src,des);
}