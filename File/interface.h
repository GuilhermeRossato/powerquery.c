#include "../_include/m_powerquery.h"

struct m_interface_file_t {
    struct m_content_t * (*Contents)(struct m_content_t * filePath);
    struct m_content_t * (*Sink)(struct m_content_t * filePath, struct m_content_t * binary); // m_File_Sink
};