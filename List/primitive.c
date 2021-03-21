#include "../_include/m_powerquery.h"

struct m_content_t * m_List_Create(struct m_content_t ** content_list, size_t content_list_size) {
    uint64_t final_size = 0;
    final_size += sizeof(struct m_interface_listcontent_t);
    final_size += (sizeof(struct m_content_t *)) * (content_list_size + 1);

    //printf("creating list of %zd size\n", content_list_size);
    for (size_t i = 0; i < content_list_size; i++) {
        final_size += sizeof(struct m_content_t) + content_list[i]->data_size;
    }
    struct m_content_t * c = m_Internal_Allocate(final_size, List.Type);
    //printf("List.Create allocated at %I64X\n", c);
    if (c == NULL) {
        printf("Could not allocate memory for object with %" PRId64 " bytes\n", final_size);
        return NULL;
    }

    struct m_interface_listcontent_t * content = c->data;

    uint8_t * content_start = (uint8_t *) content;
    uint8_t * content_end = content_start + final_size;
    //printf("List.Create: List has %lld bytes from %I64X up to %I64X for list content\n", (int64_t) final_size, (int64_t) content_start, (int64_t) content_end);

    content->list_size = content_list_size;
    content->list = (struct m_content_t **) &content[1];
    content->list[content_list_size] = NULL; // null terminator

    if (content_list_size == 0) {
        return c;
    }

    {
        // if ((void*) content->list != (void*) ptr) {
        //     printf("List.Create memory assertation failure at %s:%d\n", __FILE__, __LINE__);
        // }
        uint8_t * ptr = (uint8_t *) &content->list[content_list_size];
        ptr += sizeof(struct m_content_t *); // jump extra null pointer

        for (size_t i = 0; i < content_list_size; i++) {
            content->list[i] = (struct m_content_t *) ptr;
            // printf("The pointer[%zd] [0x%016I64X] is at 0x%016I64X\n", i, &content->list[i], (int64_t) ptr);
            ptr += sizeof(struct m_content_t) + (content_list[i]->data_size);
        }

        //printf("Internal Copy Start\n");
        for (size_t i = 0; i < content_list_size; i++) {
            struct m_content_t * pointer_to_writable_area = content->list[i];
            struct m_content_t * pointer_to_readable_area = content_list[i];
            /* @debug
            if (m_is_pointer_unwritable(pointer_to_writable_area)) {
                printf("Pointer to write (%I64X) is marked as unwritable\n", (int64_t) pointer_to_writable_area);
                exit(1);
            }
            m_add_pointer_to_unwritable(pointer_to_writable_area, sizeof(struct m_content_t) + pointer_to_readable_area->data_size);
            printf("%zd to %I64X\n", i, (int64_t) (void *) pointer_to_writable_area);
            */

            m_Internal_Copy(pointer_to_writable_area, pointer_to_readable_area);
        }

        /*
        // Check for corrupted memory or unset content_types
        for (size_t i = 0; i < content_list_size; i++) {
            if (content_list[i]->content_type != content_list[i]->content_type) {
                printf("############ THERE IS A FAILURE AT CONTENT_TYPE ##########\n");
            }
        }
        */

    }
    return c;
}