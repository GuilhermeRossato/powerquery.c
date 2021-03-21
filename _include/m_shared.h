#ifndef M_SHARED_H
#define M_SHARED_H

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

struct m_content_t {
    int64_t content_type;
    size_t data_size;
    void * data;
};

#define M_HANDLE struct m_content_t *

#include "interfaces.h"

void my_free(void * x, char * file, size_t line, char * func) {
    printf("free,,%I64X,%s,%s:%zd\n", (int64_t) x, func, file, line);
    free(x);
}

void * my_calloc(size_t size, size_t count, char * file, size_t line, char * func) {
    void * x = calloc(size, count);
    printf("calloc,%zd,%I64X,%s,%s:%zd\n", (size_t) size * count, (int64_t) x, func, file, line);
    return x;
}

void * my_malloc(size_t size, char * file, size_t line, char * func) {
    void * x = malloc(size);
    printf("malloc,%zd,%I64X,%s,%s:%zd\n", size, (int64_t) x, func, file, line);
    return x;
}

struct m_content_t * m_Internal_Allocate(size_t data_size, int content_type);

struct m_content_t * my_m_Internal_Allocate(size_t data_size, int content_type, char * file, size_t line, char * func) {
    struct m_content_t * x = m_Internal_Allocate(data_size, content_type);
    printf("m_Internal_Allocate,%zd,%I64X,%s,%s:%zd\n", data_size, (int64_t) x, func, file, line);
    return x;
}

#define DEBUG

#ifdef DEBUG
    void pq_debug_mark(char * file, int64_t line, char * func) {
        printf("%s:%lld    %s(...)\n", file, line, func);
    }

    #define free(X) my_free(X, __FILE__, __LINE__, __FUNCTION__)
    #define calloc(X, Y) my_calloc(X, Y, __FILE__, __LINE__, __FUNCTION__)
    #define malloc(X) my_malloc(X, __FILE__, __LINE__, __FUNCTION__)

#else
    #define pq_debug_mark(file, line, func)
#endif

#endif M_SHARED_H