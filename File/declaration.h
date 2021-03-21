#include "../_include/m_powerquery.h"

struct m_interface_file_t File = {0};

struct m_content_t * m_File_Contents(struct m_content_t * filePath);
struct m_content_t * m_File_Sink(struct m_content_t * filePath, struct m_content_t * binary);