#include "../_include/m_powerquery.h"
#include "primitive.c"

struct m_content_t * m_Int64_From(struct m_content_t * int64) {
    return Error.Create("Unimplemented", "Function is not implemented", NULL, __FILE__, __LINE__);
}

int m_Int64_Is(struct m_content_t * any) {
    if (any == NULL) {
        return 0;
    }
    return any->content_type == Int64.Type;
}
