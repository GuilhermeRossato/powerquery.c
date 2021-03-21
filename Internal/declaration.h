#include "../_include/m_powerquery.h"

#define md_Internal_Allocate(...) printf("%s:%lld - %s(...)", __FILE__, (uint64_t) __LINE__, __FUNCTION__); m_Internal_Allocate(__VA_ARGS__)

struct m_interface_internal_t Internal = {0};
void m_Internal_FixPointers(struct m_content_t * c);
struct m_content_t * m_Internal_Allocate(size_t data_size, int content_type);
void m_Internal_Copy(struct m_content_t * target, const struct m_content_t * origin);
