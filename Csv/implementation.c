#include "../_include/m_powerquery.h"

struct m_content_t * _m_Csv_Document_Primitive(struct m_content_t * text, struct m_content_t * delimiter, struct m_content_t * newline, struct m_content_t * columns) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    if (text == NULL || delimiter == NULL || text->content_type != Text.Type || delimiter->content_type != Text.Type || newline->content_type != Text.Type) {
        return Error.Create("InternalError", "Interfunction types mismatch", "Csv.Document(!, !)", __FILE__, __LINE__);
    }
    // Interpret CSV effectively here:
    struct m_content_t * quote = Text.Create("\"");
    struct m_content_t * quote_index = m_Text_PositionOf(text, quote, NULL, NULL);
    free(quote);
    if (quote_index != NULL) {
        free(quote_index);
        return Error.Create("Unimplemented", "CSV file with quotes are not implemented in this experimental library", "Csv.Document(!, _)", __FILE__, __LINE__);
    }

    struct m_content_t * rows = m_Text_Split(text, newline);

    if (rows == NULL || rows->data == NULL) {
        return Error.Create("InternalError", "Text.Split for newline returned NULL or NULL data", "Csv.Document(!, _)", __FILE__, __LINE__);
    } else if (rows->content_type == Error.Type) {
        return rows;
    } else if (rows->content_type != List.Type) {
        free(rows);
        return Error.Create("InternalError", "Expected Text.Split of rows to return a list of strings, but got unexpected type", "Csv.Document(!, _)", __FILE__, __LINE__);
    }

    struct m_interface_listcontent_t * row_list = (struct m_interface_listcontent_t *) rows->data;
    struct m_content_t ** row_text_list = row_list->list;
    int64_t row_size = row_list->list_size;

    // Solve (create) the column_values list of lists of text values
    struct m_content_t ** row_list_list_pointers = calloc(sizeof(struct m_content_t *), row_size);

    int64_t max_columns = -1;
    if (columns != NULL && columns->content_type == Int64.Type) {
        max_columns = Int64.Extract(columns);
    } else if (columns != NULL && columns->content_type == List.Type) {
        max_columns = ((struct m_interface_listcontent_t *) columns->data)->list_size;
    }
    for (int64_t i = 0; i < row_size; i++) {
        struct m_content_t * row_text = row_text_list[i];

        if (row_text == NULL || row_text->content_type != Text.Type) {
            free(rows);
            for (int64_t j = 0; j < i; j++) {
                free(row_list_list_pointers[j]);
            }
            free(row_list_list_pointers);
            return Error.Create("InternalError", "Unexpected Text.Split content result type extracted from content rows", "Csv.Document(!, _)", __FILE__, __LINE__);
        }

        // Check if we are at the last line of the file
        if (i + 1 == row_size) {
            // Check the classic case where a CSV ends with a trailing new line
            char * raw_row_text = m_Text_Extract(row_text);
            if (raw_row_text[0] == '\0' || (raw_row_text[0] == '\r' && raw_row_text[1] == '\0')) {
                row_list_list_pointers[i] = NULL;
                row_size--;
                break;
            }
        }

        struct m_content_t * columns = m_Text_Split(row_text, delimiter);
        int64_t column_size = ((struct m_interface_listcontent_t *) columns->data)->list_size;
        if (max_columns == -1) {
            // max_columns not specified: get the default value
            row_list_list_pointers[i] = columns;
            // also set it so that next column count always matches the expected instead of possibly varying
            max_columns = column_size;
            // treat it as if it was perfectly sized because its the definition for perfectly sized
            row_list_list_pointers[i] = columns;
            continue;
        } else if (column_size == max_columns) {
            // if the number of columns on that line matches, we don't need to splice the list
            row_list_list_pointers[i] = columns;
            continue;
        } else if (column_size < max_columns) {
            // The line has less than the amount of columns we expect
            // We must manually generate each missing column and combine it into a single list
            struct m_content_t ** content_list = calloc(sizeof(struct m_content_t *), max_columns - column_size);
            for (int64_t j = 0; j < (max_columns - column_size); j++) {
                content_list[j] = NULL;
            }
            struct m_content_t * extra = m_List_Create(content_list, max_columns - column_size);
            row_list_list_pointers[i] = m_List_Combine(columns, extra);
            if (row_list_list_pointers[i] == NULL || row_list_list_pointers[i]->content_type != List.Type || ((struct m_interface_listcontent_t *) row_list_list_pointers[i]->data)->list_size != max_columns) {
                free(rows);
                for (int64_t j = 0; j < i; j++) {
                    free(row_list_list_pointers[i]);
                }
                free(row_list_list_pointers);
                return Error.Create("InternalError", "Internal problem with list concatenation", "Csv.Document(!, !)", __FILE__, __LINE__);
            }
            free(columns);
            free(content_list);
        } else if (column_size > max_columns) {
            struct m_content_t * zero = Int64.Create(0);
            struct m_content_t * limit = Int64.Create(max_columns);
            row_list_list_pointers[i] = m_List_Range(columns, zero, limit);
            free(limit);
            free(zero);
            free(columns);
        }
    }
    free(rows);
    struct m_content_t * column_values = m_List_Create(row_list_list_pointers, row_size);

    int64_t size_of_column_list = 0;
    struct m_content_t * column_names;

    int should_free_columns_names = 0;
    // Solve (generate or assign) the column_names list depending on the configuration (record parameter)
    if (columns != NULL && columns->content_type == List.Type) {
        // Solve it when we have a LIST as column definition
        // This is simple as we just validate if it is truly made out of strings
        struct m_interface_listcontent_t * column_list = (struct m_interface_listcontent_t *) columns->data;
        for (int64_t i = 0; i < column_list->list_size; i++) {
            struct m_content_t * element = column_list->list[i];
            if (element == NULL || element->content_type != Text.Type) {
                for (int64_t i = 0; i < row_size; i++) {
                    free(row_list_list_pointers[i]);
                }
                free(row_list_list_pointers);
                return Error.Create("InvalidListContent", "Expected the supplied column list to be a string, got NULL or unmatching type", "Csv.Content(_, !)", __FILE__, __LINE__);
            }
            // We cannot let columns have duplicated values
            char * column_name_p = Text.Extract(element);
            for (int64_t j = i+1; j < row_size; j++) {
                char * other_column_name_p = Text.Extract(column_list->list[j]);
                if (Comparer.Equals(Comparer.Ordinal, column_name_p, other_column_name_p)) {
                    for (int64_t i = 0; i < row_size; i++) {
                        free(row_list_list_pointers[i]);
                    }
                    free(row_list_list_pointers);
                    char error_message[512];
                    snprintf(error_message, sizeof(error_message) - 1, "The key \"%s\" is duplicated at the column list (at index %" PRId64 " and %" PRId64 ") and that is not allowed", column_name_p, i, j);
                    return Error.Create("DuplicatedColumnName", error_message, "Csv.Document(_, !)", __FILE__, __LINE__);
                }
            }
        }
        // With everything checked, we can use de list passed in the record
        should_free_columns_names = 0;
        column_names = columns;
    } else if (columns == NULL || columns->content_type == Int64.Type) {
        if (columns == NULL) {
            if (row_size == 0) {
                size_of_column_list = 1;
            } else {
                int64_t size_of_first_line = ((struct m_interface_listcontent_t *) row_list_list_pointers[0]->data)->list_size;
                size_of_column_list = size_of_first_line;
            }
        } else {
            size_of_column_list = Int64.Extract(columns);
        }

        struct m_content_t ** default_column_list = calloc(size_of_column_list, sizeof(struct m_content_t *));
        char column_name_buffer[32];
        for (int64_t i = 0; i < size_of_column_list; i++) {
            snprintf(column_name_buffer, sizeof(column_name_buffer) - 1, "Column%" PRId64, i + 1);
            default_column_list[i] = Text.Create(column_name_buffer);
        }

        should_free_columns_names = 1;
        column_names = m_List_Create(default_column_list, size_of_column_list);
        for (int64_t i = 0; i < size_of_column_list; i++) {
            free(default_column_list[i]);
        }
        free(default_column_list);
    } else {
        for (int64_t i = 0; i < row_size; i++) {
            free(row_list_list_pointers[i]);
        }
        free(row_list_list_pointers);
        return Error.Create("InternalError", "Unhandled column parameter condition while solving the column list", "Csv.Document(_, !)", __FILE__, __LINE__);
    }

    if (column_names == NULL || column_names->content_type != List.Type) {
        for (int64_t i = 0; i < row_size; i++) {
            free(row_list_list_pointers[i]);
        }
        free(row_list_list_pointers);
        return Error.Create("SevereInternalError", "The column_names variable was not a valid list after the part of the code that should have handled it", "Csv.Document", __FILE__, __LINE__);
    }

    // Solve (create) a column_types list with full any's the same size as column_names
    struct m_content_t ** default_column_type_list = calloc(size_of_column_list, sizeof(struct m_content_t *));
    struct m_content_t * any_type_representation = Int64.Create(0);
    for (int64_t i = 0; i < size_of_column_list; i++) {
        default_column_type_list[i] = any_type_representation;
    }
    struct m_content_t * column_types = m_List_Create(default_column_type_list, size_of_column_list);

    for (int64_t i = 0; i < row_size; i++) {
        free(row_list_list_pointers[i]);
    }
    free(row_list_list_pointers);
    free(any_type_representation);
    free(default_column_type_list);

    if (column_names->content_type == Error.Type) {
        return column_names;
    } else if (column_types->content_type == Error.Type) {
        return column_types;
    } else if (column_values->content_type == Error.Type) {
        return column_values;
    }

    // Generate the list and Return
    struct m_content_t * result = m_Table_Create(column_names, column_types, column_values);

    if (should_free_columns_names) {
        free(column_names);
    }
    free(column_types);
    free(column_values);

    return result;
}

struct m_content_t * m_Csv_Document(struct m_content_t * source, struct m_content_t * optional_config, struct m_content_t * delimiter, struct m_content_t * extraValues, struct m_content_t * encoding) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    if (source == NULL) {
        return Error.Create("InvalidParameter", "The first parameter to Csv.Document should be a text or a binary type, got NULL", "Csv.Document(!, _)", __FILE__, __LINE__);
    } else if (source->content_type == Error.Type) {
        return source;
    } else if (source->content_type != Text.Type && source->content_type != Binary.Type) {
        return Error.Create("InvalidParameter", "The first parameter to Csv.Document should be a text or a binary type, got unhandled type", "Csv.Document(!, _)", __FILE__, __LINE__);
    } else if (optional_config != NULL && optional_config->content_type != Record.Type) {
        return Error.Create("InvalidParameter", "The second parameter to Csv.Document should be a record type or NULL, got unhandled type", "Csv.Document(!, _)", __FILE__, __LINE__);
    } else if (optional_config != NULL && (optional_config->data == NULL || ((struct m_interface_recordcontent_t *) optional_config->data)->keys == NULL || ((struct m_interface_recordcontent_t *) optional_config->data)->values == NULL)) {
        return Error.Create("InvalidParameter", "The config record is invalid or contains NULL pointers", "Csv.Document(_, !)", __FILE__, __LINE__);
    }
    int should_free_quotestyle = 0;
    int should_free_delimiter = 0;
    int should_free_newline = 0;
    int should_free_columns_names = 0;
    int should_free_encoding = 0;
    struct m_content_t * quotestyle = NULL;
    struct m_content_t * newline = NULL;
    struct m_content_t * columns = NULL;
    if (optional_config != NULL) {
        struct m_interface_recordcontent_t * record = (struct m_interface_recordcontent_t *) optional_config->data;
        struct m_interface_listcontent_t * keys_list = (struct m_interface_listcontent_t *) record->keys;
        struct m_interface_listcontent_t * values_list = (struct m_interface_listcontent_t *) record->values;
        for (int64_t i = 0; i < keys_list->list_size; i++) {
            if (values_list->list[i] == NULL) {
                continue;
            }
            struct m_content_t * key_obj = keys_list->list[i];
            if (key_obj == NULL || key_obj->content_type != Text.Type) {
                return Error.Create("InternalError", "Internal record struct has non-text key content on the key list", "Csv.Document(_, !)", __FILE__, __LINE__);
            }
            char * key_primitive = Text.Extract(key_obj);
            if (Comparer.Equals(Comparer.Ordinal, key_primitive, "Delimiter")) {
                if (values_list->list[i]->content_type != Text.Type) {
                    return Error.Create("InvalidRecordValue", "The 'Delimiter' property of the config must be a Text type", "Csv.Document(_, !)", __FILE__, __LINE__);
                }
                delimiter = values_list->list[i];
            } else if (Comparer.Equals(Comparer.Ordinal, key_primitive, "Columns")) {
                if (values_list->list[i]->content_type != Int64.Type && values_list->list[i]->content_type != List.Type) {
                    return Error.Create("InvalidRecordValue", "The 'Columns' property of the config must be a List type or a Int64 type", "Csv.Document(_, !)", __FILE__, __LINE__);
                }
                columns = values_list->list[i];
            } else if (Comparer.Equals(Comparer.Ordinal, key_primitive, "Encoding")) {
                encoding = values_list->list[i];
            } else if (Comparer.Equals(Comparer.Ordinal, key_primitive, "QuoteStyle")) {
                quotestyle = values_list->list[i];
            } else {
                char error_message[512];
                snprintf(error_message, sizeof(error_message) - 1, "The config parameter \"%s\" was not recognized and should not be present", key_primitive);
                return Error.Create("UnknownRecordParameter", error_message, "Csv.Document(_, !)", __FILE__, __LINE__);
            }
        }
    }
    if (quotestyle != NULL && quotestyle->content_type != Int64.Type) {
        return Error.Create("InvalidRecordValue", "The 'QuoteStyle' property of the config must be a Int64 type", "Csv.Document(_, !)", __FILE__, __LINE__);
    }
    if (source->content_type != Text.Type && encoding == NULL) {
        encoding = TextEncoding.Utf8;
    }
    if (encoding->content_type != Int64.Type) {
        return Error.Create("InvalidRecordValue", "The 'Encoding' property of the config must be a Int64 type", "Csv.Document(_, !)", __FILE__, __LINE__);
    }
    if (source->content_type == Text.Type && encoding != NULL) {
        return Error.Create("CannotChangeTextEncoding", "The encoding was defined for a CSV document of the text form, which means it has already been encoded from binary. Only binary data can be encoded.", "Csv.Document(!, !)", __FILE__, __LINE__);
    }
    int should_free_first_parameter = 0;
    if (source->content_type == Binary.Type) {
        source = Text.FromBinary(source, encoding);
        should_free_first_parameter = 1; // We allocated memory from the call above that should be freed
    }

    if (newline == NULL) {
        newline = Text.Create("\n");
        should_free_newline = 1;
    }

    if (delimiter == NULL) {
        delimiter = Text.Create(",");
        should_free_delimiter = 1;
    }

    struct m_content_t * result = _m_Csv_Document_Primitive(source, delimiter, newline, columns);
    if (should_free_newline) {
        free(newline);
    }
    if (should_free_delimiter) {
        free(delimiter);
    }
    if (should_free_first_parameter) {
        free(source);
    }
    if (should_free_encoding) {
        free(encoding);
    }
    if (result == NULL || result->content_type == Error.Type) {
        return result;
    }
    return result;
}