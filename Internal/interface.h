#include "../_include/m_powerquery.h"

struct m_interface_internal_t {
    void (*FixPointers)(struct m_content_t * c);
    struct m_content_t * (*Allocate)(size_t data_size, int content_type);
    void (*Copy)(struct m_content_t * target, const struct m_content_t * origin);
};