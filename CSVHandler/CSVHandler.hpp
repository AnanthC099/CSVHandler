#ifndef CSV_READER_HPP_INCLUDED
#define CSV_READER_HPP_INCLUDED

#include <vector>
#include <string>
#include <istream>

/**
 * Parse a single CSV line into fields.
 * @param line     The raw line, without the trailing newline.
 * @param delim    Field delimiter (defaults to comma).
 * @return         Vector of column strings.
 *
 * Guarantees:
 *  * Works with embedded delimiters and newlines inside quoted fields.
 *  * Leading/trailing spaces are preserved by design (CSV spec).
 */
std::vector<std::string>
parse_csv_line(const std::string& line, char delim = ',');

/**
 * Streamed CSV reader: iterate over rows without loading
 * the whole file in memory.
 */
class CsvReader
{
public:
    CsvReader(std::istream& in, char delim = ',')
        : m_in(in), m_delim(delim) {
    }

    /** Retrieve next row.  Returns false at EOF. */
    bool next(std::vector<std::string>& row);

private:
    std::istream& m_in;
    char           m_delim;
};

#endif // CSV_READER_HPP_INCLUDED
