#include <stdlib.h>
#include <string.h>
#include "../_include/m_shared.h"

struct m_content_t * m_Error_Create(const char * reason, const char * message, const char * details, const char * file, unsigned int line) {

    size_t string_1_size = reason != NULL ? strlen(reason) : 0;
    if (string_1_size >= 512) {
        string_1_size = 512-1;
    }
    size_t string_2_size = message != NULL ? strlen(message) : 0;
    if (string_2_size >= 512) {
        string_2_size = 512-1;
    }
    size_t string_3_size = details != NULL ? strlen(details) : 0;
    if (string_3_size >= 512) {
        string_3_size = 512-1;
    }
    size_t string_4_size = file != NULL ? strlen(file) : 0;
    if (string_4_size >= 512) {
        string_4_size = 512-1;
    }

    struct m_content_t * c = m_Internal_Allocate(sizeof(struct m_interface_errorcontent_t), Error.Type);
    struct m_interface_errorcontent_t * data = c->data;

    {
        if (reason == NULL) {
            printf("Reason was null\n");
            data->Reason[0] = '\0';
        } else {
            memcpy(data->Reason, reason, string_1_size);
            // for (int i = 0; i <= string_1_size; i++) { data->Reason[i] = reason[i]; }
        }
        if (message == NULL) {
            data->Message[0] = '\0';
        } else {
            memcpy(data->Message, message, string_2_size);
            // for (int i = 0; i <= string_2_size; i++) { data->Message[i] = message[i]; }
        }
        if (details == NULL) {
            data->Details[0] = '\0';
        } else {
            memcpy(data->Details, details, string_3_size);
            // for (int i = 0; i <= string_3_size; i++) { data->Details[i] = details[i]; }
        }
        if (file == NULL) {
            data->File[0] = '\0';
        } else {
            memcpy(data->File, file, string_4_size);
            // for (int i = 0; i <= string_4_size; i++) { data->File[i] = file[i]; }
        }
    }

    data->Reason[string_1_size] = '\0';
    data->Message[string_2_size] = '\0';
    data->Details[string_3_size] = '\0';
    data->File[string_4_size] = '\0';
    data->Line = line;


    #ifdef M_DEBUG
        printf("(debug) ");
        Debug.Print(c);
    #endif

    return c;
}

int m_Error_Is(struct m_content_t * any) {
    if (any == NULL) {
        return 0;
    }
    return any->content_type == Error.Type;
}

struct m_content_t * m_Error_Record(const char * reason, const char * message, const char * details) {
    return m_Error_Create(reason, message, details, __FILE__, __LINE__);
}