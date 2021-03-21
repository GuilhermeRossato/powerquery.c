#include "../_include/m_shared.h"

struct m_interface_error_t Error = {0};

int m_Error_Is(struct m_content_t * any);
struct m_content_t * m_Error_Create(const char * reason, const char * message, const char * details, const char * file, unsigned int line);
struct m_content_t * m_Error_Record(const char * reason, const char * message, const char * details);