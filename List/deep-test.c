#include "./../_include/m_powerquery.h"

void m_Debug_PrintRawMemory(void * rawPtr, size_t size, int isChar) {
    uint8_t * ptr = rawPtr;
    printf("[%zd bytes from raw memory (0x%zX)]\n", size, size);
    if (size < 0 || size > 512) {
        printf("Large size ignored at %s:%d\n", __FILE__, __LINE__);
        return;
    }
    printf("0x%I64X =", (int64_t) &ptr[0]);
    for (size_t i = 0; i < size; i++) {
        if (isChar) {
            printf(" %2c", (char) ptr[i]);
        } else {
            printf(" %02X", ptr[i]);
        }
        if (i % 16 == 15 && i+1 != size) {
            printf("\n");
            printf("0x%I64X =", (int64_t) &ptr[i]);
        }
    }
    if (size % 16 != 1) {
        printf("\n");
    }
}

#define SPECIFIC_TYPE_M_CONTENT_T 1
#define SPECIFIC_TYPE_M_LISTCONTENT_T 2
#define SPECIFIC_TYPE_M_INT64CONTENT_T 3
#define SPECIFIC_TYPE_M_TEXTCONTENT_T 4

#define PRINT_DEPTH for (int k = 0; k < depth; k++) { printf("\t"); }

void m_Debug_PrintSpecific(uint8_t * start, uint8_t * next, int type, int depth) {
    // printf("\n########## Start was at 0x%I64X and next was at 0x%I64X (delta is %" PRId64 ")\n\n", start, next, (int64_t) (next - start));
    if (start >= next) {
        printf("########## Start was at 0x%I64X and next was at 0x%I64X (delta is %" PRId64 ")\n", (int64_t) start, (int64_t) next, (int64_t) (next - start));
        return;
    }
    // printf("Trying to print between 0x%I64X and 0x%I64X\n", (int64_t) start, (int64_t) next);
    if (type == SPECIFIC_TYPE_M_CONTENT_T)
    {
        struct m_content_t * c = (struct m_content_t *) start;

        PRINT_DEPTH;
        printf("[");
        printf("%zd bytes", (size_t) (next - start));
        printf(" from m_content_t of type %" PRId64 "]\n", (int64_t) c->content_type);

        int64_t size = sizeof(struct m_content_t) + c->data_size;
        PRINT_DEPTH;
        printf("0x%I64X =", (uint64_t) c);
        for (size_t i = 0; i < size; i++) {
            int64_t ptr_value = (int64_t) &start[i];
            if (ptr_value == (uint64_t) &c->content_type) {
                int64_t * int_ptr = (int64_t *) &start[i];
                if (sizeof(c->content_type) == 4) {
                    if (*int_ptr == List.Type) {
                        printf(" (list  %02" PRId64 ")", (int64_t) *int_ptr);
                    } else if (*int_ptr == Text.Type) {
                        printf(" (text  %02" PRId64 ")", (int64_t) *int_ptr);
                    } else if (*int_ptr == Int64.Type) {
                        printf(" (int64 %02" PRId64 ")", (int64_t) *int_ptr);
                    } else if (*int_ptr == Error.Type) {
                        printf(" (error %02" PRId64 ")", (int64_t) *int_ptr);
                    } else {
                        printf(" (????? %02" PRId64 ")", (int64_t) *int_ptr);
                    }
                } else if (sizeof(c->content_type) == 8) {
                    if (*int_ptr == List.Type) {
                        printf(" ( list type    %6" PRId64 " )", (int64_t) *int_ptr);
                    } else if (*int_ptr == Text.Type) {
                        printf(" ( text type    %6" PRId64 " )", (int64_t) *int_ptr);
                    } else if (*int_ptr == Int64.Type) {
                        printf(" ( int64 type   %6" PRId64 " )", (int64_t) *int_ptr);
                    } else if (*int_ptr == Error.Type) {
                        printf(" ( error type   %6" PRId64 " )", (int64_t) *int_ptr);
                    } else {
                        printf(" ( ??? %15" PRId64 " )", (int64_t) *int_ptr);
                    }
                }
                i += sizeof(c->content_type)-1;
            } else if (ptr_value == (uint64_t) &c->data_size) {
                size_t * size_ptr = (size_t *) &start[i];
                if (sizeof(c->data_size) == 4) {
                    printf(" ( size%4" PRId64 " )", (int64_t) *size_ptr);
                } else if (sizeof(c->data_size) == 8) {
                    printf(" ( size           %4" PRId64 " )", (int64_t) *size_ptr);
                }
                i += sizeof(c->data_size)-1;
            } else if (ptr_value == (uint64_t) &c->data) {
                if (sizeof(c->data) == 4) {
                    printf(" (x%I64X)", (int64_t) c->data);
                } else if (sizeof(c->data) == 8) {
                    printf(" (  0x%016I64X )", (int64_t) c->data);
                }
                i += sizeof(c->data)-1;
            } else if (ptr_value == (uint64_t) c->data) {
                printf("\n");
                if (c->content_type == List.Type) {
                    m_Debug_PrintSpecific(c->data, next, SPECIFIC_TYPE_M_LISTCONTENT_T, depth+1);
                } else if (c->content_type == Int64.Type) {
                    m_Debug_PrintSpecific(c->data, next, SPECIFIC_TYPE_M_INT64CONTENT_T, depth+1);
                } else if (c->content_type == Text.Type) {
                    m_Debug_PrintSpecific(c->data, next, SPECIFIC_TYPE_M_TEXTCONTENT_T, depth+1);
                } else {
                    printf("Calling PrintRawMemory from %s:%d\n", __FILE__, __LINE__);
                    m_Debug_PrintRawMemory(c->data, c->data_size, 0);
                }
                i += c->data_size - 1;
            } else {
                printf(" %02x", start[i]);
            }
            if (i % 16 == 15 && i+1 != size) {
                printf("\n");
                PRINT_DEPTH;
                printf("0x%I64X =", (uint64_t) &start[i]);
            }
            if (size < 0 || size > 256) {
                printf("\n");
                return;
            }
        }

        if (size % 16 != 15) {
            printf("\n");
        }
    }
    else if (type == SPECIFIC_TYPE_M_INT64CONTENT_T)
    {
        int64_t * c = start;

        PRINT_DEPTH;
        printf("[");
        printf("%zd bytes", next - start);
        printf(" from int64_t]\n");
        PRINT_DEPTH;
        printf("0x%I64X =", (uint64_t) c);
        printf(" (  0x%016I64X )\n", *c);
    }
    else if (type == SPECIFIC_TYPE_M_LISTCONTENT_T)
    {
        struct m_interface_listcontent_t * c = start;

        PRINT_DEPTH;
        printf("[%zd bytes from m_interface_listcontent_t]\n", sizeof(struct m_interface_listcontent_t));

        PRINT_DEPTH;
        printf("0x%I64X =", (int64_t) start);

        size_t size = c->list_size;

        printf(" ( list size %9zi )", size);

        int64_t * ptr_to_list_array_number = (int64_t *) &c->list;

        printf(" (  0x%016I64X ) <- pointer to list of pointers with %zi pointers\n", *ptr_to_list_array_number, size);

        for (size_t i = 0; i < c->list_size; i++) {
            if (i % 2 == 0) {
                PRINT_DEPTH;
                printf("0x%I64X =", (int64_t) c->list);
            }
            struct m_content_t * element = c->list[i];
            printf(" (  0x%016I64X )", (int64_t) element);
            if (i % 2 == 1) {
                printf("\n");
            }
        }
        if (c->list_size % 2 != 0) {
            printf("\n");
        }
        for (size_t i = 0; i < c->list_size; i++) {
            struct m_content_t * element = c->list[i];
            struct m_content_t * next_element = i+1 < c->list_size ? c->list[i + 1] : ((struct m_content_t *) next);
            // printf("\n\nAbout to call printspecific for element %zd\n\n", i);
            m_Debug_PrintSpecific((uint8_t *) element, (uint8_t *) next_element, SPECIFIC_TYPE_M_CONTENT_T, depth+1);
        }
    }
    else if (type == SPECIFIC_TYPE_M_TEXTCONTENT_T)
    {
        struct m_interface_textcontent_t * c = start;

        PRINT_DEPTH;
        printf("[%zd bytes from m_interface_textcontent_t]\n", sizeof(struct m_interface_listcontent_t));

        PRINT_DEPTH;
        printf("0x%I64X =", (int64_t) start);

        int encoding = c->encoding;

        if (encoding == m_Int64_Extract(TextEncoding.Ascii)) {
            printf(" (  Ascii  )");
        } else {
            printf(" ( en%5d )", encoding);
        }

        int64_t size = c->length;

        printf(" ( %13" PRId64 " chars )", size);

        char * primitive = (char *) c->buffer;
        printf(" (  0x%016I64X )\n", (uint64_t) primitive);

        PRINT_DEPTH;
        printf("0x%I64X =", (int64_t) primitive);
        for (int64_t i = 0; i <= size; i++) {
            if (primitive[i] == '\0') {
                printf(" \\0");
            } else if (i == size) {
                printf(" !!");
            } else {
                printf(" %2c", primitive[i]);
            }
        }
    }
    else
    {
        PRINT_DEPTH;
        printf("Unsupported at %s:%d\n", __FILE__, __LINE__);
    }
}

void m_Debug_PrintMemory(struct m_content_t * c) {
    uint8_t * ptr = c;
    ptr += sizeof(struct m_content_t);
    ptr += c->data_size;
    m_Debug_PrintSpecific(c, (uint8_t *) ptr, SPECIFIC_TYPE_M_CONTENT_T, 0);
}

int m_Int64_VerifyIntegrity(struct m_content_t * c) {
    if (c == NULL) {
        return -1;
    } else if (c->content_type == Error.Type) {
        return -2;
    } else if (c->content_type != Int64.Type) {
        return -3;
    } else if (c->data_size != sizeof(int64_t)) {
        return -4;
    } else if (c->data == NULL) {
        return -5;
    }

    uint8_t * data = (uint8_t *) c->data;
    uint8_t * ptr = (uint8_t *) c;
    ptr += sizeof(struct m_content_t);
    if (ptr != c->data) {
        return -6;
    }
    return 1;
}

void test() {
    struct m_content_t * num1 = m_Int64_Create(0xFF00FFAA00FF00AA);
    struct m_content_t * num2 = m_Int64_Create(0xAAAAAAAAFFFFFFFF);

    printf("Integrity num1 = %d\n", m_Int64_VerifyIntegrity(num1));
    printf("Integrity num2 = %d\n", m_Int64_VerifyIntegrity(num2));

    struct m_content_t * values[] = {num1, num2, num1};
    struct m_content_t * list = m_List_Create(values, sizeof(values) / sizeof(*values));

    printf("The list is stored at ");
    printf("0x%016I64X", (uint64_t) list);
    printf("\n");

    printf("################ Memory ################ \n");
    m_Debug_PrintRawMemory((void *) list, (size_t) sizeof(struct m_content_t) + list->data_size, 0);
    printf("################ Inteligent Memory ################ \n");
    m_Debug_PrintMemory(list);
    printf("################ Debug Print ################ \n");
    m_Debug_Print(list);
    printf("################ Test Ending ################ \n");

    printf("The list is stored at ");
    printf("0x%016I64X", (uint64_t) list);
    printf("\n");

    for (int i = 0; i < m_Int64_Extract(List.Count(list)); i++) {
        struct m_content_t * num1_r = m_List_Get(list, m_Int64_Create(i));
        m_Debug_Print(num1_r);
        printf("\tIntegrity num%d_r = %d\n", i, m_Int64_VerifyIntegrity(num1_r));
        printf("\tValue: 0x%016I64X\n", m_Int64_Extract(num1_r));
    }

    printf("################ Test End ################ \n");
    return;
}