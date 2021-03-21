#include "../_include/m_powerquery.h"

struct m_interface_csv_t {
    /**
     * Returns the contents of the CSV document as a table.
     *
     * If the configuration record is not specified (NULL), the default values are set, otherwise, the configuration parameter can receive the following record keys:
     *    Delimiter: The column delimiter. Default: ",".
     *    Columns: Can be null, the number of columns, a list of column names, or a table type. If the number of columns is lower than the number found in the input, the additional columns will be ignored. If the number of columns is higher than the number found in the input, the additional columns will be null. When not specified, the number of columns will be determined by what is found in the input.
     *    Encoding: The text encoding of the file. Default: TextEncoding.ASCII.
     *    QuoteStyle: Specifies how quoted line breaks are handled. QuoteStyle.None (default): All line breaks are treated as the end of the current row, even when they occur inside a quoted value. QuoteStyle.Csv: Quoted line breaks are treated as part of the data, not as the end of the current row.
     *
     * @param binary_or_text The binary or text content to be interpreted
     * @param optional_config The record to configurate the file interpretation, if not specified, everything is defined as the default.
     */
    struct m_content_t * (*Document)(struct m_content_t * source, struct m_content_t * optional_config, struct m_content_t * delimiter, struct m_content_t * extraValues, struct m_content_t * encoding);
};
