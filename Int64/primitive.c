#include "../_include/m_powerquery.h"

struct m_content_t * m_Int64_Create(int64_t value) {
    struct m_content_t * c = m_Internal_Allocate(sizeof(int64_t), Int64.Type);
    if (!c) {
        return NULL;
    }
    uint64_t * ptr = c->data;
    *ptr = value;
    return c;
}

int64_t m_Int64_Extract(struct m_content_t * int64) {
    if (int64 == NULL) {
        printf("Warning: cannot extract int64 from NULL\n");
        return 0;
    } else if (int64->content_type != Int64.Type) {
        printf("Warning: cannot extract int64 from a different type\n");
        return 0;
    }
    uint64_t * ptr = int64->data;
    return *ptr;
}