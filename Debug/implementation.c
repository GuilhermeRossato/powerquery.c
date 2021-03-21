#include <stdlib.h>
#include "../_include/m_powerquery.h"

#define m_Debug_PrintStruct_depth for(int k = 1 ; k <= depth; k++ ) { printf("    "); }

void m_Debug_PrintStruct(struct m_content_t * c, int depth) {
    int64_t * content_type = &c->content_type;
    size_t * data_size = &c->data_size;
    uint8_t * data = (uint8_t *) &c->data;

    m_Debug_PrintStruct_depth;
    printf("[ (0x%I64X)\n", (int64_t) c);

    depth++;
    m_Debug_PrintStruct_depth;
    printf("0x%I64X = content_type = 0x%I64X\n", (int64_t) content_type, (int64_t) *content_type);
    m_Debug_PrintStruct_depth;
    printf("0x%I64X = data_size = 0x%I64X\n", (int64_t) data_size, (int64_t) *data_size);
    m_Debug_PrintStruct_depth;
    printf("0x%I64X = data = \n", (int64_t) data);
    m_Debug_PrintStruct_depth;
    depth--;

    m_Debug_PrintStruct_depth;
    printf("]\n");
}

void m_Debug_PrintRawMemory(void * rawPtr, size_t size, int is_char) {
    uint8_t * ptr = rawPtr;
    printf("[%zd bytes]\n", size);
    printf("0x%I64X =", (int64_t) &ptr[0]);
    for (size_t i = 0; i < size; i++) {
        if (is_char) {
            printf(" %2c", (char) ptr[i]);
        } else {
            printf(" %02x", ptr[i]);
        }
        if (i % 16 == 15) {
            printf("\n");
            printf("0x%I64X =", (int64_t) &ptr[i]);
        }
    }
    if (size % 16 != 0) {
        printf("\n");
    }
}

void m_Debug_PrintTable(struct m_content_t * c, int should_be_full) {
    if (c == NULL || c->content_type != Table.Type || c->data == NULL) {
        printf("Cannot print non-table content\n");
        return;
    }

    struct m_interface_tablecontent_t * table = c->data;
    int64_t column_count = *table->column_count;
    int64_t row_count = *table->row_count;

    int64_t max_column_shown = should_be_full ? column_count : (column_count >= 10 ? 10 : column_count);
    int64_t max_row_shown = should_be_full ? row_count : (row_count >= 10 ? 10 : row_count);

    int64_t * max_columns_lengths = calloc(max_column_shown, sizeof(int64_t));
    for (int64_t column = 0; column < max_column_shown; column++) {
        max_columns_lengths[column] = 0;
    }

    int64_t max_index_count = max_row_shown < 10 ? 1 : (max_row_shown < 100 ? 2 : (max_row_shown < 1000 ? 3 : (max_row_shown < 10000 ? 4 : (max_row_shown < 100000 ? 5 : (max_row_shown < 1000000 ? 6 : 7)))));

    // printf("There are %" PRId64 " rows and %" PRId64 " columns\n", max_row_shown, max_column_shown);

    char buffer[256];
    // Figure the max column lengths
    for (int64_t row = -1; row < max_row_shown; row++) {
        for (int64_t column = 0; column < max_column_shown; column++) {
            struct m_content_t * element;
            {
                struct m_content_t * list_obj;
                if (row == -1) {
                    list_obj = table->names_list;
                } else {
                    list_obj = table->values_list;
                }
                if (list_obj == NULL || list_obj->content_type != List.Type || list_obj->data == NULL) {
                    free(max_columns_lengths);
                    printf("Error: Internal list from table content at ");
                    if (row == -1) {
                        printf("table header", row);
                    } else {
                        printf("row %" PRId64, row);
                    }
                    printf(" is not a valid list\n");
                    return;
                }
                /* Debug show row if its not header
                if (row != -1) {
                    printf("Showing row %" PRId64 "\n", row);
                    m_Debug_PrintComplexity(list_obj, 2);
                }
                */
                struct m_interface_listcontent_t * list_obj_content = (struct m_interface_listcontent_t *) list_obj->data;
                if (row == -1) {
                    if (column < 0 || column >= list_obj_content->list_size) {
                        free(max_columns_lengths);
                        printf("Error: Trying to access index %" PRId64 " from names_list that contains %" PRId64 " elements\n", column, list_obj_content->list_size);
                        return;
                    }
                    element = list_obj_content->list[column];
                    if (element == NULL || element->content_type != Text.Type || element->data == NULL) {
                        free(max_columns_lengths);
                        printf("Error: Table header column name list has an element that is not a valid Text object at index %" PRId64 "\n", column);
                        return;
                    }
                } else {
                    if (row < 0 || row >= list_obj_content->list_size) {
                        free(max_columns_lengths);
                        printf("Error: Trying to access index %" PRId64 " from row list that contains %" PRId64 " elements\n", row, list_obj_content->list_size);
                        return;
                    }
                    struct m_content_t * row_obj = list_obj_content->list[row];
                    if (row_obj == NULL || row_obj->content_type != List.Type || row_obj->data == NULL) {
                        free(max_columns_lengths);
                        printf("Row at index %" PRId64 " did not have a valid list of columns as data\n", row);
                        return;
                    }
                    struct m_interface_listcontent_t * row_list = row_obj->data;

                    if (column < 0 || column >= row_list->list_size) {
                        free(max_columns_lengths);
                        printf("Error: Trying to access index %" PRId64 " from row of index %" PRId64 " that contains %" PRId64 " elements\n", column, row, row_list->list_size);
                        return;
                    }

                    element = row_list->list[column];

                    /* Debug show row-column value
                    printf("# row %" PRId64 ", column %" PRId64 " has element of type %" PRId64 "\n", row, column, element == NULL ? 0 : element->content_type);
                    m_Debug_Print(element);
                    */
                }
            }
            int64_t length = 0;
            if (element == NULL) {
                length = 5; // [NULL]
            } else if (element->data == NULL) {
                length = 11; // [Data NULL]
            } else if (element->content_type == Text.Type) {
                length = ((struct m_interface_textcontent_t *) element->data)->length;
                // printf("text is %s\n", ((struct m_interface_textcontent_t *) element->data)->buffer);
                if (length > 28) {
                    length = 28;
                }
            } else if (element->content_type == Int64.Type) {
                snprintf(buffer, sizeof(buffer) - 1, "%" PRId64, *((int64_t *) element->data));
                length = strlen(buffer);
                if (length > 28) {
                    length = 28;
                }
            } else if (element->content_type == Error.Type) {
                length = 8 + strlen(((struct m_interface_errorcontent_t *) element->data)->Message);
                if (length > 28) {
                    length = 28;
                }
            } else if (element->content_type == Record.Type) {
                length = 8; // [Record]
            } else if (element->content_type == List.Type) {
                length = 6; // [List]
            } else if ((element->content_type) == Binary.Type) {
                length = 8; // [Binary]
            } else {
                length = 9; // [Unknown]
            }
            if (length > max_columns_lengths[column]) {
                max_columns_lengths[column] = length;
                // printf("At row %" PRId64 " and column %" PRId64 " we updated the size of the column to %" PRId64 "\n", row, column, length);
            } else {
                // printf("At row %" PRId64 " and column %" PRId64 " we did not update the size of the column to %" PRId64 "\n", row, column, length);
            }
        }
    }
    for (int64_t row = -3; row <= max_row_shown; row++) {
        if (row == -3 || row == -1 || row == max_row_shown) {
            printf("|");
            for (int64_t column = 0; column < max_column_shown; column++) {
                printf("-");
                for (int64_t j = 0; j < max_columns_lengths[column]; j++) {
                    printf("-");
                }
                printf("-|");
            }
            printf("\n");
        } else if (row == -2) {
            // header
            struct m_content_t * list_obj = table->names_list;
            struct m_interface_listcontent_t * list_obj_content = (struct m_interface_listcontent_t *) list_obj->data;
            printf("|");
            for (int64_t column = 0; column < max_column_shown; column++) {
                printf(" ");
                struct m_content_t * element = list_obj_content->list[column];
                char * text = m_Text_Extract(element);
                snprintf(buffer, sizeof(buffer) - 1, "%s", m_Text_Extract(element));
                buffer[28] = '\0';
                int hasFinished = 0;
                for (int64_t j = 0; j < max_columns_lengths[column]; j++) {
                    if (text[j] == '\0') {
                        hasFinished = 1;
                    }
                    if (hasFinished) {
                        printf(" ");
                    } else {
                        printf("%c", text[j]);
                    }
                }
                printf(" |");
            }
            printf("\n");
        } else {
            printf("|");
            struct m_content_t * list_obj = table->values_list;
            struct m_interface_listcontent_t * list_obj_content = (struct m_interface_listcontent_t *) list_obj->data;
            for (int64_t column = 0; column < max_column_shown; column++) {
                printf(" ");
                struct m_content_t * row_obj = list_obj_content->list[row];
                struct m_interface_listcontent_t * row_list = row_obj->data;
                struct m_content_t * element = row_list->list[column];
                buffer[0] = '?';
                buffer[1] = '\0';
                if (element == NULL) {
                    snprintf(buffer, sizeof(buffer) - 1, "[NULL]");
                } else if (element->data == NULL) {
                    snprintf(buffer, sizeof(buffer) - 1, "[nNULL]");
                } else if (element->content_type == Text.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "%s", ((struct m_interface_textcontent_t *) element->data)->buffer);
                    buffer[28] = '\0';
                } else if (element->content_type == Int64.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "%" PRId64, *((int64_t *) element->data));
                    buffer[28] = '\0';
                } else if (element->content_type == Error.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "%s", ((struct m_interface_errorcontent_t *) element->data)->Message);
                    buffer[28] = '\0';
                } else if (element->content_type == Record.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "[Record]");
                } else if (element->content_type == List.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "[List]");
                } else if ((element->content_type) == Binary.Type) {
                    snprintf(buffer, sizeof(buffer) - 1, "[Binary]");
                } else {
                    snprintf(buffer, sizeof(buffer) - 1, "[Unknown]");
                }
                int hasFinished = 0;
                for (int64_t j = 0; j < max_columns_lengths[column]; j++) {
                    if (buffer[j] == '\0') {
                        hasFinished = 1;
                    }
                    if (hasFinished) {
                        printf(" ");
                    } else {
                        printf("%c", buffer[j]);
                    }
                }
                printf(" |");
            }
            printf("\n");
        }
    }
    free(max_columns_lengths);
}

void m_Debug_PrintComplexity(struct m_content_t * c, int complexity) {
    if (c == NULL) {
        printf("NULL\n");
    } else if (c->content_type == Error.Type) {
        struct m_interface_errorcontent_t * data = c->data;
        printf("[Error] %s\n", data->Message);
        if (complexity > 0) {
            if (data->Reason && data->Reason[0] != '\0') {
                printf("\tReason: \"%s\"\n", data->Reason);
            }
            if (data->File && data->File[0] != '\0') {
                printf("\tAt: \"%s:%d\"\n", data->File, data->Line);
            }
            if (data->Details && data->Details[0] != '\0') {
                printf("\tDetails: \"%s\"\n", data->Details);
            }
        }
    } else if (c->content_type == Binary.Type) {
        if (c->data_size == 1) {
            printf("[Binary] 1 byte\n");
        } else {
            printf("[Binary] %" PRId64 " bytes\n", c->data_size);
        }
    } else if (c->content_type == Int64.Type) {
        int64_t v = m_Int64_Extract(c);
        printf("[Int64] %" PRId64 "\n", v);
    } else if (c->content_type == Text.Type) {
        const char * buffer = m_Text_Extract(c);
        printf("[Text] \"%s\"\n", buffer);
    } else if (c->content_type == List.Type) {
        printf("[List] ");
        struct m_interface_listcontent_t * data = c->data;
        if (complexity <= 0 || data->list_size == 0) {
            printf("%zd elements\n", data->list_size);
        } else {
            printf("%zd elements:\n", data->list_size);
            int64_t max_shown = data->list_size >= 10 ? 10 : data->list_size;
            for (int64_t i = 0; i < max_shown; i++) {
                printf("\t[%" PRId64 "] = ", i);
                struct m_content_t * list_content_ptr = data->list[i];
                if (list_content_ptr == NULL) {
                    printf("[Null Terminator]\n");
                    continue;
                }
                if (complexity >= 2) {
                    if (list_content_ptr != NULL && list_content_ptr->content_type == List.Type) {
                        struct m_interface_listcontent_t * sub_data = list_content_ptr->data;
                        if (sub_data == NULL) {
                            printf("[List object with null data]\n");
                            continue;
                        } else if (sub_data->list == NULL) {
                            printf("[List content with null list pointer]\n");
                            continue;
                        } else if (sub_data->list_size == 0) {
                            printf("[List] {}");
                            continue;
                        }
                        printf("[List] {");
                        int64_t sub_max_list = (complexity >= 3) ? data->list_size : (data->list_size >= 10 ? 10 : data->list_size);
                        for (int64_t j = 0; j < sub_max_list; j++) {
                            struct m_content_t * sub_content = sub_data->list[j];
                            if (sub_content == NULL) {
                                printf("[NULL]");
                            } else if (sub_content->data == NULL) {
                                printf("[Data NULL]");
                            } else if (sub_content->content_type == Int64.Type) {
                                printf("%" PRId64, m_Int64_Extract(sub_content));
                            } else if (sub_content->content_type == Text.Type) {
                                printf("\"%s\"", m_Text_Extract(sub_content));
                            } else if (sub_content->content_type == List.Type) {
                                printf("[List]");
                            } else if (sub_content->content_type == Record.Type) {
                                printf("[Record]");
                            } else if (sub_content->content_type == Table.Type) {
                                printf("[Table]");
                            } else {
                                printf("[Unknown]");
                            }
                            if (j+1 != sub_max_list) {
                                printf(", ");
                            }
                        }
                        printf("}\n");
                    } else {
                        m_Debug_PrintComplexity(list_content_ptr, 0);
                    }
                } else {
                    m_Debug_PrintComplexity(list_content_ptr, 0);
                }
            }
        }
    } else if (c->content_type == Record.Type) {
        printf("[Record]");
        if (complexity <= 0) {
            printf("\n");
        } else {
            struct m_interface_recordcontent_t * data = c->data;
            struct m_content_t * keys = data->keys;
            struct m_content_t * values = data->values;
            printf(" ");
            if (keys == NULL) {
                printf("Missing keys\n");
            } else if (values == NULL) {
                printf("Missing values\n");
            } else if (keys->content_type != List.Type) {
                printf("Invalid record keys type: %" PRId64 "\n", (int64_t) keys->content_type);
            } else if (values->content_type != List.Type) {
                printf("Invalid record values type: %" PRId64 "\n", (int64_t) values->content_type);
            } else if (values->data == NULL || keys->data == NULL) {
                printf("Invalid null data for values or keys\n");
            } else if (((struct m_interface_listcontent_t *) values->data)->list_size != ((struct m_interface_listcontent_t *) keys->data)->list_size) {
                printf("Record has mismatch as there are %" PRId64 " keys for %" PRId64 " values\n", ((struct m_interface_listcontent_t *) keys->data)->list_size, ((struct m_interface_listcontent_t *) values->data)->list_size);
            } else {
                struct m_interface_listcontent_t * values_list = (struct m_interface_listcontent_t *) values->data;
                struct m_interface_listcontent_t * keys_list = (struct m_interface_listcontent_t *) keys->data;
                printf("%" PRId64 " properties:\n", values_list->list_size);
                int64_t max_shown = complexity >= 2 ? values_list->list_size : (values_list->list_size >= 10 ? 10 : values_list->list_size);

                for (int64_t i = 0; i < max_shown; i++) {
                    struct m_content_t * key = keys_list->list[i];
                    if (key->content_type != Text.Type) {
                        printf("\t<invalid key at %" PRId64 " of type %" PRId64 "> = ", i, (int64_t) key->content_type);
                    } else {
                        printf("\t\"%s\" = ", ((struct m_interface_textcontent_t *) key->data)->buffer);
                    }
                    struct m_content_t * value = values_list->list[i];
                    if (value->content_type == Text.Type && ((struct m_interface_textcontent_t *) value->data)->encoding == Int64.Extract(TextEncoding.Ascii)) {
                        printf("\"%s\"\n", m_Text_Extract(value));
                    } else if (value->content_type == Int64.Type) {
                        printf("%" PRId64 "\n", m_Int64_Extract(value));
                    } else {
                        m_Debug_PrintComplexity(value, 0);
                    }
                }
            }
        }
    } else if (c->content_type == Table.Type) {
        if (complexity <= 0) {
            printf("[Table]\n");
        } else {
            printf("[Table]");

            struct m_interface_tablecontent_t * table = c->data;
            if (table->column_count == NULL) {
                printf(" Column count null and ");
            } else if (*table->column_count == 1) {
                printf(" 1 column and ");
            } else {
                printf(" %" PRId64 " columns and ", *table->column_count);
            }
            if (table->row_count == NULL) {
                printf("row count null\n");
            } else if (*table->row_count == 1) {
                printf(" 1 row\n");
            } else {
                printf("%" PRId64 " rows\n", *table->row_count);
            }
            if (complexity >= 2 && table->column_count != NULL && table->row_count != NULL) {
                m_Debug_PrintTable(c, complexity >= 3);
            }
        }
    } else {
        printf("[Unknown]\n");
    }
}

void m_Debug_PrintSimple(struct m_content_t * content) {
    m_Debug_PrintComplexity(content, 0);
}

void m_Debug_Print(struct m_content_t * content) {
    m_Debug_PrintComplexity(content, 1);
}

void m_Debug_PrintFull(struct m_content_t * content) {
    m_Debug_PrintComplexity(content, 2);
}