#include "../_include/m_powerquery.h"

int test() {
    for (int i = 0; i < 1; i++) {
        _sleep(100);
        //struct m_content_t * file = Text.Create("");
        //struct m_content_t * content = File.Contents(file);
        struct m_content_t * text = Text.Create("Hello There,World\nWhat,IsUp\n"); // later test with newline at the end
        struct m_content_t * result = Csv.Document(text, NULL, NULL, NULL, NULL);
        m_Debug_PrintFull(result);
        free(text);
        free(result);
    }
    return 1;
}