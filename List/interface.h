#include "../_include/m_powerquery.h"

struct m_interface_listcontent_t {
    int64_t list_size;
    struct m_content_t ** list;
};

struct m_interface_list_t {
    int Type;

    struct m_content_t * (*Count)(struct m_content_t * content_list);
    struct m_content_t * (*Get)(struct m_content_t * any, struct m_content_t * index);
    struct m_content_t * (*Create)(struct m_content_t ** content_list, size_t content_list_size);
    struct m_content_t * (*Range)(struct m_content_t * list, struct m_content_t * offset, struct m_content_t * optional_count);

    int (*Is)(struct m_content_t * any);
};