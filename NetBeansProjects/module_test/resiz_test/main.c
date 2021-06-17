/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2012年3月3日, 下午1:51
 */

#include <stdio.h>
#include <stdlib.h>

#include "libresiz.h"

/*
 * 
 */
int main(int argc, char** argv) {

    int used_size;

    /*
        int result = mkntfs("C:\\SDUserSpace\\UserPool.sdi", "xxxxx");
        if (result) return -1;   
     */

    int result = ntfsinfo(&used_size, "C:\\SDUserSpace\\UserPool.sdi");
    if (result) {
        printf("result:%d", result);
        return -1;
    }
    printf("used_size:%d", used_size);

    // int result = ntfsresiz("D:\\SDUserSpace\\UserPool.sdi", nNewDisksize << 10);
    // if (result) return -1;

    return (EXIT_SUCCESS);
}

