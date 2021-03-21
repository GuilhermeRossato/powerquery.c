#include "../_include/m_powerquery.h"

void m_Internal_FixPointers(struct m_content_t * c) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    if (c == NULL) {
        return;
    }
    if (c->data != &c[1]) {
        printf("Warning: Internal.FixPointers had to fix content data pointer\n");
        c->data = &c[1];
    }
    if (c->content_type == Text.Type) {
        struct m_interface_textcontent_t * content = c->data;
        struct m_interface_textcontent_t * target = &content[1];
        content->buffer = (char *) target;
    } else if (c->content_type == List.Type) {
        struct m_interface_listcontent_t * content = c->data;
        struct m_interface_listcontent_t * target = &content[1];


        for (uint64_t i = 0; i <= content->list_size; i++) {
            struct m_content_t * element = content->list[i];
            if (i == content->list_size) {
                if (element != NULL) {
                    content->list[i] = NULL;
                }
            } else if (element == NULL) {
                printf("Warning: NULL pointer to element on list index %" PRId64 " detected at \"%s:%d\"\n", i, __FILE__, __LINE__);
            } else {
                m_Internal_FixPointers(element);
            }
        }

    } else if (c->content_type == Record.Type) {
        struct m_interface_recordcontent_t * content = c->data;

        uint8_t * ptr = (uint8_t *) (&content[1]);
        content->values = (struct m_content_t *) ptr;
        ptr += sizeof(struct m_content_t) + content->values->data_size;
        content->keys = (struct m_content_t *) ptr;

        m_Internal_FixPointers(content->values);
        m_Internal_FixPointers(content->keys);
    } else if (c->content_type == Table.Type) {
        struct m_interface_tablecontent_t * content = c->data;

        uint8_t * ptr = (void *) content;
        ptr += sizeof(struct m_interface_tablecontent_t);
        content->names_list = (void *) ptr;
        struct m_content_t * column_names = content->names_list;
        ptr += sizeof(struct m_content_t);
        ptr += column_names->data_size;
        content->types_list = (void *) ptr;
        struct m_content_t * column_types = content->types_list;
        ptr += sizeof(struct m_content_t);
        ptr += column_types->data_size;
        content->values_list = (void *) ptr;
        struct m_content_t * column_values = content->values_list;
        ptr += sizeof(struct m_content_t);
        ptr += column_values->data_size;

        struct m_interface_listcontent_t * column_names_list = (struct m_interface_listcontent_t *) column_names;
        struct m_interface_listcontent_t * column_types_list = (struct m_interface_listcontent_t *) column_types;
        struct m_interface_listcontent_t * column_values_list = (struct m_interface_listcontent_t *) column_values;

        int64_t * row_count = &column_values_list->list_size;
        int64_t * column_count = &column_names_list->list_size;

        content->row_count = row_count;
        content->column_count = column_count;

        int64_t delta_ptr = (int64_t) ((int64_t) ptr) - ((int64_t) content);

        size_t data_size = sizeof(struct m_interface_tablecontent_t) + (sizeof(struct m_content_t) * 3) + column_names->data_size + column_types->data_size + column_values->data_size;
        if (delta_ptr != data_size) {
            printf("Diff didnt match: delta_ptr %" PRId64 " vs data_size %" PRId64 "\n", delta_ptr, data_size);
        }

        m_Internal_Copy(content->names_list, column_names);
        m_Internal_Copy(content->types_list, column_types);
        m_Internal_Copy(content->values_list, column_values);
    } else if (c->content_type != Int64.Type) {
        printf("%s:%d\n", __FILE__, __LINE__);
        printf("FATAL ERROR: Cannot fix pointers for unknow type: %" PRId64 "\n", c->content_type);
        printf("Program will now exit\n");
        exit(1);
    }
}

struct m_content_t * m_Internal_Allocate(size_t data_size, int content_type) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    size_t malloc_size = sizeof(struct m_content_t) + data_size;
    // printf("######## Will allocate %4zd bytes (0x%X)\n", malloc_size, (unsigned int) malloc_size);
    struct m_content_t * c = calloc(1, malloc_size);
    // printf("######## Allocated %4zd bytes (0x%X) at %12I64x for object of id %2d\n", malloc_size, (unsigned int) malloc_size, (uint64_t) c, content_type);

    if (c == NULL) {
        return NULL;
    }
    c->content_type = content_type;
    c->data_size = data_size;

    uint8_t * ptr = (uint8_t *) c;
    ptr += sizeof(struct m_content_t);
    c->data = ptr;

    for (int i = 0; i < data_size; i++) {
        ptr[i] = 0;
    }

    return c;
}

void m_Internal_Copy(struct m_content_t * target, const struct m_content_t * origin) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    target->content_type = origin->content_type;
    target->data_size = origin->data_size;

    uint8_t * ptr = (uint8_t *) target;
    ptr += sizeof(struct m_content_t);
    target->data = ptr;

    memcpy(target->data, origin->data, origin->data_size);

    // printf("Copied %lld bytes of %I64X to %lld bytes of %I64X\n", (int64_t) sizeof(struct m_content_t) + origin->data_size, (int64_t) origin, (int64_t) sizeof(struct m_content_t) + target->data_size, (int64_t) target);

    m_Internal_FixPointers(target);
}

