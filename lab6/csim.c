/* 
 * csim.c - 一个可以重放Valgrind跟踪文件并输出统计信息（如命中次数、未命中次数和
 * 驱逐次数）的缓存模拟器。
 * 替换策略为最近最少使用（LRU）。
 *
 * 实现和假设：
 *  1. 每次加载/存储最多只能引起一次缓存未命中。（我检查了跟踪文件，
 *  看到的最大请求是8字节）。
 *  2. 指令加载（I）被忽略，因为我们感兴趣的是评估trans.c的数据缓存性能。
 *  3. 数据修改（M）被视为先加载后存储到同一地址的操作。因此，M操作可能导致两次缓存命中，
 *  或者一次未命中和一次命中加上可能的驱逐。
 *
 * 函数printSummary()用于打印输出。
 * 请使用此函数打印命中次数、未命中次数和驱逐次数。
 * 这对于驱动程序评估您的工作至关重要。
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cachelab.h"

// 定义 DEBUG_ON
#define ADDRESS_LENGTH 64 // 定义地址长度为64位

/* 类型定义：内存地址 */
typedef unsigned long long int mem_addr_t; // 定义一个类型为无符号长长整型
                                            //，用于表示内存地址

/* 类型：缓存行
   LRU 是用于实现最近最少使用（Least Recently Used）替换策略的计数器 */
typedef struct cache_line {
    char valid;//有效位
    mem_addr_t tag;//tag标记位
    unsigned long long int lru;//计数位
} cache_line_t;

typedef cache_line_t* cache_set_t;//cache_set_t代表一组缓存行
typedef cache_set_t* cache_t;//cache_t代表整个缓存结构


/* 由命令行参数设置的全局变量 */
int verbosity = 0; /* 如果设置，则打印跟踪信息 */
int s = 0; /* 集合索引位 */
int b = 0; /* 块偏移位 */
int E = 0; /* 关联度 */
char* trace_file = NULL; /* 跟踪文件 */



/* 从命令行参数派生的变量 */
int S; /* 集合的数量 */
int B; /* 块大小（字节） */

/* 用于记录缓存统计数据的计数器 */
int miss_count = 0; /* 未命中次数 */
int hit_count = 0; /* 命中次数 */
int eviction_count = 0; /* 驱逐次数 */
unsigned long long int lru_counter = 1; /* LRU计数器 */

/* 我们正在模拟的缓存 */
cache_t cache;  
mem_addr_t set_index_mask; // 集合索引掩码

/* 
 * initCache - 分配内存，将有效位、标记和LRU写为0
 * 同时计算set_index_mask
 */
void initCache()
{
    int i,j;
    cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S);
    for (i=0; i<S; i++){
        cache[i]=(cache_line_t*) malloc(sizeof(cache_line_t) * E);
        for (j=0; j<E; j++){
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lru = 0;
        }
    }

    /* 计算组索引 */
    set_index_mask = (mem_addr_t) (pow(2, s) - 1);
}


/* 
 * freeCache - 释放分配的内存
 */
void freeCache()
{
    int i;
    for (i=0; i<S; i++){
        free(cache[i]);
    }
    free(cache);
}


/* 
 * accessData - 访问内存地址 addr 处的数据。
 *   如果该数据已经在缓存中，增加 hit_count（命中计数）。
 *   如果该数据不在缓存中，将其加载到缓存中，并增加 miss_count（缺失计数）。
 *   如果有缓存行被替换，增加 eviction_count（替换计数）。
 */

void accessData(mem_addr_t addr)
{




}


/*
 * replayTrace - 重放给定的跟踪文件与缓存
 */
void replayTrace(char* trace_fn)//trace_fn跟踪文件名
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if(verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            accessData(addr);

            /* If the instruction is R/W then access again */
            if(buf[1]=='M')
                accessData(addr);
            
            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - 打印使用信息
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
    char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
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
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Compute S, E and B from command line args */
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);
 
    /* Initialize cache */
    initCache();

#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
    printf("DEBUG: set_index_mask: %llu\n", set_index_mask);
#endif
 
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
