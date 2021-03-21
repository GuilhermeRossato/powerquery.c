#include "../_include/m_shared.h"

struct m_interface_debug_t Debug = {0};

void m_Debug_Print(struct m_content_t * content);

void m_Debug_PrintComplexity(struct m_content_t * c, int complexity);
void m_Debug_PrintSimple(struct m_content_t * content);
void m_Debug_PrintFull(struct m_content_t * content);
void m_Debug_PrintRawMemory(void * rawPtr, size_t size, int is_char);
void m_Debug_PrintStruct(struct m_content_t * c, int depth);