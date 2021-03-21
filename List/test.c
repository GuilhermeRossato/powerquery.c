#include "../_include/m_powerquery.h"

void primitive_test() {
    int64_t primitive_value= 12;
    char primitive_string[] = "Bob";
    char primitive_digits[] = "123-4567";

    struct m_content_t * num0 = Int64.Create(0);
    struct m_content_t * num1 = Int64.Create(1);
    struct m_content_t * num2 = Int64.Create(2);

    for (int i = 0; i < 10; i++) {
        struct m_content_t * first_element = Int64.Create(primitive_value);
        struct m_content_t * second_element = Text.Create(primitive_string);
        struct m_content_t * third_element = Text.Create(primitive_digits);
        struct m_content_t * primitive_list[] = {first_element, second_element, third_element};
        struct m_content_t * list = List.Create(primitive_list, 3);
        free(second_element);
        free(third_element);
        struct m_content_t * list_size = List.Count(list);

        if (!Int64.Is(list_size)) {
            printf("Invalid list size\n");
            Debug.Print(list_size);
            return;
        } else if (Int64.Extract(list_size) != 3) {
            printf("Expected list size to be 3\n");
            return;
        }

        struct m_content_t * new_first_element = List.Get(list, num0);
        struct m_content_t * new_second_element = List.Get(list, num1);
        struct m_content_t * new_third_element = List.Get(list, num2);

        if (new_first_element == first_element) {
            printf("List element was not copied, but referenced\n");
            return;
        }
        free(new_first_element);
        if (new_first_element == NULL || new_first_element->content_type != Int64.Type) {
            printf("First list element was unexpected\n");
            Debug.Print(new_first_element);
            return;
        } else if (new_second_element == NULL || new_second_element->content_type != Text.Type) {
            printf("Second list element was unexpected\n");
            Debug.Print(new_second_element);
            return;
        } else if (new_third_element == NULL || new_third_element->content_type != Text.Type) {
            printf("Third list element was unexpected\n");
            Debug.Print(new_third_element);
            return;
        }

        free(new_first_element);
        free(new_second_element);
        free(new_third_element);
    }
}

void test() {
    int64_t primitiveInt = 12;
    M_HANDLE firstElement = Int64.Create(primitiveInt);
    char bob[] = "Bob";
    M_HANDLE secondElement = Text.Create(bob);
    char * bobCopy = Text.Extract(secondElement);

    if (bobCopy == bob) {
        printf("Text.Create did not copy char array, instead it used the pointer!\n");
        return;
    } else if (bobCopy[0] != bob[0]) {
        printf("Text.Create did not copy the char array correctly");
        return;
    }

    M_HANDLE thirdElement = Text.Create("123-4567");

    M_HANDLE primitiveList[] = {firstElement, secondElement, thirdElement};
    M_HANDLE list = List.Create(primitiveList, 3);

    M_HANDLE listSize = List.Count(list);

    if (!Int64.Is(listSize)) {
        Debug.Print(listSize);
        return;
    }

    int64_t size_p = Int64.Extract(listSize);

    if (size_p != 3) {
        printf("List has unexpected size");
        return;
    }

    struct m_content_t * newFirstElement = List.Get(list, Int64.Create(0));
    struct m_content_t * newSecondElement = List.Get(list, Int64.Create(1));
    struct m_content_t * newThirdElement = List.Get(list, Int64.Create(2));

    if (newFirstElement == firstElement) {
        printf("List did not create a copy of first element on retrieval\n");
        return;
    }

    if (newFirstElement->content_type != firstElement->content_type) {
        printf("First element type mismatch\n");
        return;
    }

    if (newFirstElement->data_size != firstElement->data_size) {
        printf("First element data_size mismatch\n");
        return;
    }

    int64_t firstElement_p = Int64.Create(newFirstElement);
    char * secondElement_p = Text.Create(newSecondElement);
    char * thirdElement_p = Text.Create(newThirdElement);

    if (secondElement_p == bob) {
        printf("List element should not have pointed to original char array\n");
        return;
    } else if (secondElement_p == bobCopy) {
        printf("List element should not have pointed to original retrieved char array\n");
        return;
    }

    if (firstElement_p != primitiveInt) {
        printf("The retrieved first element integer does not match the original value\n");
        return;
    } else if (secondElement_p[0] != bob[0] || secondElement_p[1] != bob[1] || secondElement_p[2] != bob[2]) {
        printf("The retrieved second element string does not match the original value\n");
        return;
    }


    struct m_content_t * elem = Text.Create("Hi");

    printf("Finished with no errors\n");
}