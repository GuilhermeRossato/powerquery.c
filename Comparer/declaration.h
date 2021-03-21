#include "../_include/m_shared.h"

struct m_interface_comparer_t Comparer = {0};

int m_Comparer_Ordinal(const char * x, const char * y);
int m_Comparer_OrdinalIgnoreCase(const char * x, const char * y);
int m_Comparer_Equals(int (*comparer)(const char *, const char *), const char * x, const char * y);
