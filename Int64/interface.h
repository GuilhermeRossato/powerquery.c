#include "../_include/m_powerquery.h"

struct m_interface_int64_t {
    int Type;

    struct m_content_t * (*From)(struct m_content_t *);
    int (*Is)(struct m_content_t *);

    struct m_content_t * (*Create)(int64_t value);
    int64_t (*Extract)(struct m_content_t *);
};