#include "../_include/m_powerquery.h"

struct m_interface_list_t List = {0};

struct m_content_t * m_List_Count(struct m_content_t * list);
struct m_content_t * m_List_Get(struct m_content_t * list, struct m_content_t * index);
struct m_content_t * m_List_Create(struct m_content_t ** content_list, size_t content_list_size);