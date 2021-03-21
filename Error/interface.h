#include "../_include/m_powerquery.h"

struct m_interface_error_t {
    int Type;
    struct m_content_t * (*Record)(const char * reason, const char * message, const char * detail);
    struct m_content_t * (*Create)(const char * reason, const char * message, const char * detail, const char * file, unsigned int line);
    int (*Is)(struct m_content_t * any);
};

struct m_interface_errorcontent_t {
    char Reason[512];
    char Message[512];
    char Details[512];
    char File[512];
    unsigned int Line;
};
