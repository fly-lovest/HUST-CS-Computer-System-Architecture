#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"

typedef struct Cacheline{
    int valid;
    unsigned int tag;
    int count;
} Cacheline;

int hit = 0;
int miss = 0;
int eviction = 0;
int opt,s,E,b,t;  //s表示组索引位数，b表示块偏移位，E表示每一组的行数，t表示tag位数
int S,B,T;
Cacheline **Cache;
int search_Cache(unsigned address){
    unsigned tag = (address>>(s+b))&(T-1);
    unsigned group = (address>>b)&(S-1);
    //printf("address=%x tag=%x group=%x",address,tag,group);
    int i,j;
    int flag = 0;
    for(j=0;j<E;j++){
        if(Cache[group][j].valid == 1){
            Cache[group][j].count++;
        }
    }
    for(i=0;i<E;i++){
        if(Cache[group][i].valid == 1 && Cache[group][i].tag == tag){
            Cache[group][i].count = 0;
            hit++;
            //printf("hit\n");
            return 0;
        }
    }
    miss++;

    for(i=0;i<E;i++){
        if(Cache[group][i].valid == 0){
            Cache[group][i].tag = tag;
            Cache[group][i].valid = 1;
            Cache[group][i].count = 0;
            flag = 1;
            break;
        }
    }
    if(flag == 1) {
        //printf("miss\n");
        return 1;
    }
    eviction++;
    int max_count = -1;  //替换最大计数值
    int eindex = -1;          //替换编号
    for(i=0;i<E;i++){
        if(Cache[group][i].count>max_count){
            max_count = Cache[group][i].count;
            eindex = i;
        }
    }
    Cache[group][eindex].tag = tag;
    Cache[group][eindex].valid = 1;
    Cache[group][eindex].count = 0;
    //printf("miss eviction\n");
    return 2;
}


int main(int argc, char** argv){
    int i,j;
    char trace[30];
    while((opt = getopt(argc, argv, "hvs:E:b:t:"))!=-1){  //处理参数
        switch(opt){
            case 's':  
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(trace,optarg);
            default:
                break;
        }
    }
    S = 1<<s;
    B = 1<<b;
    t = 32-s-b;
    T = 1<<t;
    //printf("S=%d B=%d E=%d T=%d s=%d b=%d t=%d\n",S,B,E,T,s,b,t);
    Cache = (Cacheline**) malloc(sizeof(Cacheline*)*S);
    for(i=0;i<S;i++){
        Cache[i] = (Cacheline*)malloc(sizeof(Cacheline)*E);
    }
    for(i=0;i<S;i++){
        for(j=0;j<E;j++){
            Cache[i][j].valid = 0;
            Cache[i][j].tag = -1;
            Cache[i][j].count = 0;
        }
    }
    FILE* fp = fopen(trace,"r");
    char operation;
    unsigned address;
    int size;
    while(fscanf(fp," %c %x,%u",&operation, &address, &size)>0){
        if(operation == 'L'){
            search_Cache(address);
        }
        else if(operation == 'S'){
            search_Cache(address);
        }
        else if(operation == 'M'){
            search_Cache(address);
            search_Cache(address);
        }
    }
    fclose(fp);
    //printf("%d %d %d\n",hit, miss, eviction);
    printSummary(hit, miss, eviction);
    return 0;
}