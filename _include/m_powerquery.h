#ifndef M_POWERQUERY_H
#define M_POWERQUERY_H

#include "m_shared.h"

/*
int64_t unwritables_start[128];
int64_t unwritables_end[128];
int unwritable_size;

void m_add_pointer_to_unwritable(void * ptr, int64_t size) {
    if (unwritable_size >= (sizeof(unwritables_start) / sizeof(int64_t))) {
        printf("Cannot add more unwritables\n");
    }
    unwritables_start[unwritable_size] = (int64_t) ptr;
    unwritables_end[unwritable_size] = unwritables_start[unwritable_size] + size;
    unwritable_size++;
}

int m_is_pointer_unwritable(void * ptr) {
    for (int i = 0; i < unwritable_size; i++) {
        if (((int64_t) ptr) >= unwritables_start[i]) {
            if (((int64_t) ptr) < unwritables_end[i]) {
                return 1;
            }
        }
    }
    return 0;
}
*/

#include "declarations.h"
#include "implementations.h"

int m_powerquery_has_initialized = 0;

void m_init() {
    if (m_powerquery_has_initialized == 1) {
        return;
    }
    m_powerquery_has_initialized = 1;

    int type = 1;
    Error.Type = type++;
    Binary.Type = type++;
    Text.Type = type++;
    Int64.Type = type++;
    List.Type = type++;
    Table.Type = type++;
    Record.Type = type++;

    Internal.Allocate = m_Internal_Allocate;
    Internal.Copy = m_Internal_Copy;
    Internal.FixPointers = m_Internal_FixPointers;

    TextEncoding.Ascii = m_Int64_Create(20127);
    TextEncoding.BigEndianUnicode = m_Int64_Create(1201);
    TextEncoding.Unicode = m_Int64_Create(1200);
    TextEncoding.Utf16 = m_Int64_Create(1200);
    TextEncoding.Utf8 = m_Int64_Create(65001);
    TextEncoding.Windows = m_Int64_Create(1252);

    Error.Is = m_Error_Is;
    Error.Create = m_Error_Create;
    Error.Record = m_Error_Record;

    File.Contents = m_File_Contents;
    File.Sink = m_File_Sink;

    Text.Is = m_Text_Is;
    Text.From = m_Text_From;
    Text.FromBinary = m_Text_FromBinary;
    Text.Split = m_Text_Split;
    Text.StartsWith = m_Text_StartsWith;
    Text.Range = m_Text_Range;
    Text.Length = m_Text_Length;
    Text.Create = m_Text_Create;
    Text.Extract = m_Text_Extract;

    Debug.Print = m_Debug_Print;
    Debug.PrintRawMemory = m_Debug_PrintRawMemory;

    Int64.Is = m_Int64_Is;
    Int64.From = m_Int64_From;
    Int64.Create = m_Int64_Create;
    Int64.Extract = m_Int64_Extract;

    List.Is = m_List_Is;
    List.Create = m_List_Create;
    List.Count = m_List_Count;
    List.Range = m_List_Range;

    Comparer.Ordinal = m_Comparer_Ordinal;
    Comparer.OrdinalIgnoreCase = m_Comparer_OrdinalIgnoreCase;
    Comparer.Equals = m_Comparer_Equals;

    Occurrence.First = m_Int64_Create(0);
    Occurrence.Last = m_Int64_Create(1);
    Occurrence.All = m_Int64_Create(2);

    QuoteStyle.None = m_Int64_Create(0);
    QuoteStyle.Csv = m_Int64_Create(1);

    Table.Is = m_Table_Is;
    Table.Create = m_Table_Create;
    Table.ColumnCount = m_Table_ColumnCount;
    Table.Column = m_Table_Column;
    Table.RowCount = m_Table_RowCount;

    Csv.Document = m_Csv_Document;

    Record.Is = m_Record_Is;
    Record.FromList = m_Record_FromList;
    Record.Field = m_Record_Field;
    Record.HasField = m_Record_HasField;
    Record.FieldOrDefault = m_Record_FieldOrDefault;
}

#endif