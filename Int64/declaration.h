#include "../_include/m_powerquery.h"

struct m_interface_int64_t Int64 = {0};

struct m_content_t * m_Int64_From(struct m_content_t * int64);
int m_Int64_Is(struct m_content_t * any);

struct m_content_t * m_Int64_Create(int64_t value);
int64_t m_Int64_Extract(struct m_content_t * int64);