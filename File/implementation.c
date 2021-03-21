#include <stdio.h>
#include "../_include/m_powerquery.h"

struct m_content_t * m_File_Contents(struct m_content_t * filePath) {
    if (filePath == NULL) {
        return Error.Create("ParameterMismatch", "The first parameter to File.Contents cannot be null", NULL, __FILE__, __LINE__);
    } else if (filePath->content_type == Error.Type) {
        return filePath;
    } else if (filePath->content_type != Text.Type) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "The first parameter to File.Contents must be a Text instance, got type id " PRId64, filePath->content_type);
        return Error.Create("ParameterMismatch", messageBuffer, NULL, __FILE__, __LINE__);
    }
    struct m_interface_textcontent_t * content = filePath->data;
    int encoding_p = m_Int64_Extract(TextEncoding.Ascii);
    if (content->encoding != encoding_p) {
        return Error.Create("NotImplemented", "Only the Ascii encoding is implemented at File.Contents", NULL, __FILE__, __LINE__);
    }
    const char * c_filePath = content->buffer;

    FILE * fp = fopen(c_filePath, "rb");
    if (!fp) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Could not read or find the file \"%s\" to get its content", c_filePath);
        return Error.Create("FileNotFound", messageBuffer, c_filePath, __FILE__, __LINE__);
    }
    fseek(fp, 0L, SEEK_END);
    long content_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    struct m_content_t * c = m_Internal_Allocate(content_size + 1, Binary.Type);
    c->data_size = content_size;
    char * buffer = c->data;
    size_t read_bytes = fread(buffer, 1, content_size, fp);
    if (read_bytes != content_size) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Could not read file bytes correctly, got %zi bytes out of %i expected", read_bytes, content_size);
        free(c);
        fclose(fp);
        return Error.Create("IOFailure", messageBuffer, c_filePath, __FILE__, __LINE__);;
    }
    buffer[c->data_size] = '\0';
    fclose(fp);
    return c;
}

struct m_content_t * m_File_Sink(struct m_content_t * filePath, struct m_content_t * binary) {
    if (filePath == NULL) {
        return Error.Create("ParameterMismatch", "First parameter of File.Sink must be a text string with the path to the new file, got null", "File.Sink", __FILE__, __LINE__);
    } else if (filePath->content_type == Error.Type) {
        return filePath;
    } else if (filePath->content_type != Text.Type) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "First parameter of File.Sink must be a text handle type (%d), got %d", (int) Text.Type, (int) filePath->content_type);
        return Error.Create("ParameterMismatch", messageBuffer, "File.Sink(!, _)", __FILE__, __LINE__);
    } else if (binary == NULL) {
        return Error.Create("ParameterMismatch", "Second parameter of File.Sink must be a text or a binary handle with the file content, got null", "File.Sink", __FILE__, __LINE__);
    } else if (binary->content_type == Error.Type) {
        return binary;
    }

    char * buffer;
    size_t buffer_size;
    if (binary->content_type == Binary.Type) {
        buffer = binary->data;
        buffer_size = binary->data_size;
    } else if (binary->content_type == Text.Type) {
        buffer = m_Text_Extract(binary);
        buffer_size = binary->data_size;
    } else {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Second parameter of File.Sink must be a text (%d) or a binary handle type (%d), got %d", (int) Text.Type, (int) Binary.Type, (int) binary->content_type);
        return Error.Create("ParameterMismatch", messageBuffer, "File.Sink(_, !)", __FILE__, __LINE__);
    }

    const char * c_filePath = m_Text_Extract(filePath);
    FILE * fp = fopen(c_filePath, "w");
    if (!fp) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Error: File at \"%s\" could not be open for writing\n", c_filePath);
        fclose(fp);
        return Error.Create("IOFailure", messageBuffer, c_filePath, __FILE__, __LINE__);
    }
    size_t written_bytes = fwrite(buffer, 1, buffer_size, fp);

    if (written_bytes != buffer_size) {
        char messageBuffer[1024];
        snprintf(messageBuffer, sizeof(messageBuffer) - 1, "Failed to write the binary of size %zi to file, only %zi bytes written to \"%s\"\n", written_bytes, buffer_size, c_filePath);
        fclose(fp);
        return Error.Create("IOFailure", messageBuffer, c_filePath, __FILE__, __LINE__);
    }

    struct m_content_t * c = m_Int64_Create(written_bytes);
    fclose(fp);
    return c;
}