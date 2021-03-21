#include "../_include/m_powerquery.h"

struct m_interface_debug_t {
    void (*Print)(struct m_content_t *);
    void (*PrintRawMemory)(void * rawPtr, size_t size, int is_char);
};
