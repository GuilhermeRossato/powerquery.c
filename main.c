#include <stdio.h>
#include <stdlib.h>

#include "_include/m_powerquery.h"
#include "Record/test.c"

/*
struct m_interface_t;
extern struct m_interface_Error_t Error;
extern struct m_interface_file_t File;
extern struct m_interface_binary_t Binary;
extern struct m_interface_text_t Text;
extern struct m_interface_text_encoding_t TextEncoding;
*/

int main(void) {
    m_init();

    test();
/*
    M_HANDLE filePath = Text.Create("C:/Users/gui_r/dev/cpowerquery/main.c");
    M_HANDLE binContent = File.Contents(filePath);
    printf("Text from Binary\n");
    M_HANDLE txt = Text.FromBinary(binContent, TextEncoding.Ascii);
    M_HANDLE newline = Text.Create("\n");
    printf("Splitting\n");
    M_HANDLE rows = m_Text_Split(txt, newline);
    printf("Debugging\n");

    Debug.Print(rows);

    printf("Finished\n");
    free(binContent);
    //free(txt);

/*
    M_HANDLE txt = Text.FromBinary(binContent, TextEncoding.Utf8);
    HANDLE doc = Csv.Document(txt);

    HANDLE data = Table.PromoteHeaders(doc);

    HANDLE list = Table.Column(data, "CustomerId");

    char * text = Text.Combine(list, ", ");

    printf("List is %s\n", text);

    // The language requires us to clean stuff up:

    Free.Handle(binContent);
    Free.Handle(txt);
    Free.Handle(data);
    Free.Handle(list);
    Free.Handle(text);

*/
    printf("End\n");
    return 0;
}
