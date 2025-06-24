#ifndef CSV_TABLE_HPP_INCLUDED
#define CSV_TABLE_HPP_INCLUDED
/* ---------------------------------------------------------------- *
 *  In‑memory table with header‑name lookup (C++98 only)            *
 * ---------------------------------------------------------------- */
#include "csv_reader_unicode.hpp"
#include <map>
#include <vector>
#include <stdexcept>

template <typename CharT>
class CsvTable
{
public:
    typedef CsvReaderT<CharT>                    reader_type;
    typedef typename reader_type::row_type       row_type;
    typedef std::basic_string<CharT>             string_t;

    CsvTable(typename reader_type::istream_type& in,
        CharT delim = static_cast<CharT>(','))      // load whole file
    {
        reader_type rdr(in, delim);
        row_type row;
        if (!rdr.next(row))
            throw std::runtime_error("CSV: empty input");

        m_header = row;
        for (std::size_t i = 0; i < row.size(); ++i)
            m_index[row[i]] = i;

        while (rdr.next(row))
            m_rows.push_back(row);
    }

    std::size_t row_count() const { return m_rows.size(); }
    std::size_t col_count() const { return m_header.size(); }

    const string_t&
        operator()(const string_t& col, std::size_t row) const
    {
        typename std::map<string_t, std::size_t>::const_iterator it =
            m_index.find(col);
        if (it == m_index.end())
            throw std::out_of_range("CSV: unknown column");
        if (row >= m_rows.size())
            throw std::out_of_range("CSV: row out of range");

        std::size_t c = it->second;
        return (c < m_rows[row].size()) ? m_rows[row][c] : m_empty;
    }

private:
    row_type                        m_header;
    std::vector<row_type>           m_rows;
    std::map<string_t, std::size_t>  m_index;

    static const string_t           m_empty;
};

template <typename CharT>
const std::basic_string<CharT> CsvTable<CharT>::m_empty =
std::basic_string<CharT>();

#endif /* CSV_TABLE_HPP_INCLUDED */
