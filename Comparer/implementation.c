#include <stdlib.h>
#include "../_include/m_shared.h"

int m_Comparer_Ordinal(const char * x, const char * y) {
    unsigned int i;
    for (i = 0; x[i] != '\0' && y[i] != '\0'; i++) {
        if (x[i] != y[i]) {
            return 0;
        }
    }
    if (x[i] == y[i]) {
        return 1;
    }
    return 0;
}

int m_Comparer_OrdinalIgnoreCase(const char * x, const char * y) {
    unsigned int i;
    for (i = 0; x[i] != '\0' && y[i] != '\0'; i++) {
        if (x[i] >= 'a' && x[i] <= 'z' && y[i] >= 'A' && y[i] <= 'Z') {
            if (x[i] - 'a' != y[i] - 'A') {
                return 0;
            }
        } else if (x[i] >= 'A' && x[i] <= 'Z' && y[i] >= 'a' && y[i] <= 'z') {
            if (x[i] - 'A' != y[i] - 'a') {
                return 0;
            }
        } else if (x[i] != y[i]) {
            return 0;
        }
    }
    if (x[i] == y[i]) {
        return 1;
    }
    return 0;
}

int m_Comparer_Equals(int (*comparer)(const char *, const char *), const char * x, const char * y) {
    return comparer(x, y);
}