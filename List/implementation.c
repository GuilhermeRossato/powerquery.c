#include "./../_include/m_powerquery.h"
#include "primitive.c"

struct m_content_t * m_List_Count(struct m_content_t * list) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    if (list == NULL) {
        return Error.Create("InvalidParameter", "First parameter of List.Count must be a list, got null", "List.Count", __FILE__, __LINE__);
    } else if (list->content_type == Error.Type) {
        return list;
    } else if (list->content_type != List.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "First parameter of List.Count must be a list handle type (%d), got %d", (int) List.Type, (int) list->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Count(!)", __FILE__, __LINE__);
    }
    struct m_interface_listcontent_t * content = list->data;
    return m_Int64_Create(content->list_size);
}

struct m_content_t * _m_List_Get(struct m_content_t * list, uint64_t index_raw) {
    if (list == NULL) {
        return NULL;
    } else if (list->content_type == Error.Type) {
        return list;
    } else if (list->content_type != List.Type) {
        return NULL;
    }
    struct m_interface_listcontent_t * content = list->data;
    if (index_raw < 0 || index_raw >= content->list_size) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "List index %" PRId64 " does not exist in list with %" PRId64 " values", index_raw, (int64_t) content->list_size);
        return Error.Create("ListIndexNotFound", error_message, "List.Get(_, !)", __FILE__, __LINE__);
    }
    if (content->list == NULL) {
        return Error.Create("InvalidInternalState", "Content list does not have a pointer to its content", "List.Get", __FILE__, __LINE__);
    }
    struct m_content_t ** list_instance = content->list;
    struct m_content_t * element = list_instance[index_raw];
    if (element == NULL) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "List index %" PRId64 " was not found in the list with %" PRId64 " values", index_raw, (int64_t) content->list_size);
        return Error.Create("ListIndexNotFound", error_message, "List.Get(_, !)", __FILE__, __LINE__);
    }
    if (element->content_type <= 0 || element->content_type > 50) {
        m_Debug_PrintRawMemory(list, sizeof(struct m_content_t) + list->data_size, 0);
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "List index %" PRId64 " had unexpected content type of %" PRId64 "", index_raw, (int64_t) element->content_type);
        return Error.Create("ListIndexNotFound", error_message, "List.Get(_, !)", __FILE__, __LINE__);
    }

    struct m_content_t * c = m_Internal_Allocate(element->data_size, element->content_type);
    m_Internal_Copy(c, element);
    return c;
}

struct m_content_t * m_List_Get(struct m_content_t * list, struct m_content_t * index) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    if (list == NULL) {
        return Error.Create("InvalidParameter", "First parameter of List.Get must be a list, got null", "List.Get(!, _)", __FILE__, __LINE__);
    } else if (list->content_type == Error.Type) {
        return list;
    } else if (list->content_type != List.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "First parameter of List.Get must be a list handle type (%d), got %d", (int) List.Type, (int) list->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Get(!, _)", __FILE__, __LINE__);
    } else if (index == NULL) {
        return Error.Create("InvalidParameter", "Second parameter of List.Get must be a list, got null", "List.Get(_, !)", __FILE__, __LINE__);
    } else if (index->content_type == Error.Type) {
        return index;
    } else if (index->content_type != Int64.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "Second parameter of List.Get must be a Int64 handle type (%d), got %d", (int) Int64.Type, (int) index->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Get(_, !)", __FILE__, __LINE__);
    }
    int64_t p = m_Int64_Extract(index);
    if (p < 0) {
        return Error.Create("NegativeListIndex", "The list index cannot be negative", "List.Get(_, !)", __FILE__, __LINE__);
    }
    return _m_List_Get(list, (uint64_t) p);
}

struct m_content_t *  m_List_Range(struct m_content_t * list, struct m_content_t * offset, struct m_content_t * optional_count) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    return Error.Create("UNTESTED", "UNTESTED", "List.Range", __FILE__, __LINE__);
    if (list == NULL) {
        return Error.Create("InvalidParameter", "First parameter (list) of List.Range must be a list, got null", "List.Range(!, _)", __FILE__, __LINE__);
    } else if (list->content_type == Error.Type) {
        return list;
    } else if (list->content_type != List.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "First parameter (list) of List.Range must be a list handle type (%d), got %d", (int) List.Type, (int) list->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Range(!, _)", __FILE__, __LINE__);
    } else if (offset == NULL) {
        return Error.Create("InvalidParameter", "Second parameter (offset) of List.Range must be a list, got null", "List.Range(_, !)", __FILE__, __LINE__);
    } else if (offset->content_type == Error.Type) {
        return offset;
    } else if (offset->content_type != Int64.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "Second parameter (offset) of List.Range must be a Int64 handle type (%d), got %d", (int) Int64.Type, (int) offset->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Range(_, !)", __FILE__, __LINE__);
    } else if (optional_count != NULL && optional_count->content_type == Error.Type) {
        return optional_count;
    } else if (optional_count != NULL && optional_count->content_type != Int64.Type) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "Third parameter (count) of List.Range must be a Int64 handle type (%d), got %d", (int) Int64.Type, (int) optional_count->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Range(_, _, !)", __FILE__, __LINE__);
    } else if (optional_count == NULL && optional_count->content_type == Int64.Type && Int64.Extract(optional_count) < 0) {
        static char error_message[1024];
        snprintf(error_message, sizeof(error_message) - 1, "Third parameter (count) of List.Range must be a positive integer, got %" PRId64, Int64.Extract(optional_count));
        return Error.Create("InvalidParameter", error_message, "List.Range(_, _, !)", __FILE__, __LINE__);
    }

    int64_t offset_p = Int64.Extract(offset);
    int64_t list_length = ((struct m_interface_listcontent_t *) list->data)->list_size;

    int64_t count_p = optional_count == NULL ? (list_length - offset_p) : Int64.Extract(optional_count);

    struct m_content_t ** new_list_primitive = calloc(sizeof(struct m_content_t *), count_p);

    if (count_p == 0) {
        return m_List_Create(NULL, 0);
    }

    struct m_content_t ** list_objects = ((struct m_interface_listcontent_t *) list->data)->list;
    for (int64_t i = 0; i < count_p; i++) {
        if ((i + offset_p) < 0 || i >= count_p || (i + offset_p) >= list_length) {
            printf("Warning: Skipping invalid index %" PRId64 "\n", i + offset_p);
            continue;
        }
        new_list_primitive[i] = list_objects[offset_p + i];
    }

    struct m_content_t * new_list = List.Create(new_list_primitive, count_p);
    free(new_list_primitive);
    return new_list;
}

int m_List_Is(struct m_content_t * any) {
    if (any == NULL) {
        return 0;
    }
    return any->content_type == List.Type;
}

struct m_content_t * m_List_Combine(struct m_content_t * lists) {
    pq_debug_mark(__FILE__, __LINE__, __FUNCTION__);
    static char error_message[1024];
    if (lists == NULL) {
        return Error.Create("InvalidParameter", "List.Combine expects a list, got null", "List.Combine(!)", __FILE__, __LINE__);
    } else if (lists->content_type == Error.Type) {
        return lists;
    } else if (lists->content_type != List.Type) {
        snprintf(error_message, sizeof(error_message) - 1, "List.Combine expects a list of handle type %d, got %d", (int) List.Type, (int) lists->content_type);
        return Error.Create("InvalidParameter", error_message, "List.Combine(!)", __FILE__, __LINE__);
    } else if (lists->data == NULL) {
        return Error.Create("InvalidParameter", "List.Combine first parameter has null data", "List.Combine(!)", __FILE__, __LINE__);;
    }
    struct m_interface_listcontent_t * content = lists->data;
    size_t final_size = 0;
    for (int64_t i = 0; i < content->list_size; i++) {
        if (content->list[i] == NULL) {
            snprintf(error_message, sizeof(error_message) - 1, "List.Combine expects each list element to be a list, got NULL at index %lld ou of %lld", i, content->list_size);
            return Error.Create("InvalidParameter", error_message, "List.Combine(!)", __FILE__, __LINE__);
        } else if (content->list[i]->content_type == Error.Type) {
            snprintf(error_message, sizeof(error_message) - 1, "List.Combine expects each list element to be a list, got error at index %lld ou of %lld", i, content->list_size);
            return Error.Create("InvalidParameter", error_message, "List.Combine(!)", __FILE__, __LINE__);
        } else if (content->list[i]->content_type != List.Type) {
            snprintf(error_message, sizeof(error_message) - 1, "List.Combine expects each list element to be a list with handle type %d, got %d", List.Type, content->list[i]->content_type);
            return Error.Create("InvalidParameter", error_message, "List.Combine(!)", __FILE__, __LINE__);
        } else if (content->list[i]->data == NULL) {
            snprintf(error_message, sizeof(error_message) - 1, "List.Combine expects each list element to be a list, got element with NULL data at index %lld ou of %lld", i, content->list_size);
            return Error.Create("InvalidParameter", error_message, "List.Combine(!)", __FILE__, __LINE__);
        }
        struct m_content_t * inner = content->list[i]->data;
        struct m_interface_listcontent_t * inner_list = inner->data;
        final_size += inner_list->list_size;
        if (final_size <= 0) {
            return Error.Create("Overflow", "List combination caused an overflow in size_t, making it impossible to allocate enough memory to hold the data", "List.Combine(!)", __FILE__, __LINE__);
        }
    }
    int64_t index = 0;
    struct m_content_t ** content_list = calloc(sizeof(struct m_content_t *), final_size);
    for (int64_t i = 0; i < content->list_size; i++) {
        struct m_content_t * inner = content->list[i]->data;
        struct m_interface_listcontent_t * inner_list = inner->data;
        for (size_t j = 0; j < inner_list->list_size; j++) {
            if (index < final_size) {
                content_list[index] = inner_list->list[j];
            }
            index++;
        }
    }
    struct m_content_t * result = m_List_Create(content_list, final_size);
    free(content_list);
    return result;
}