#include "../_include/m_powerquery.h"

struct m_interface_comparer_t {

    int (*Ordinal)(const char *, const char *);
    int (*OrdinalIgnoreCase)(const char *, const char *);

    int (*Equals)(int (*comparer)(const char *, const char *), const char *, const char *);
};
