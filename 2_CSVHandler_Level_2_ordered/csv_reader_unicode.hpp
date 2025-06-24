#ifndef CSV_READER_UNICODE_HPP_INCLUDED
#define CSV_READER_UNICODE_HPP_INCLUDED
/* ---------------------------------------------------------------------- *
 *  Generic, C++98‑only CSV parser supporting char, wchar_t, …            *
 * -------------------------------------------------------------------- */

#include <vector>
#include <string>
#include <istream>
#include <cstdlib>   // size_t

 /* ---------- helper: narrow char literal → CharT ----------------------- */
template <typename CharT>
inline CharT make_ch(char c) { return static_cast<CharT>(c); }

/* ---------- parse a logical CSV line into fields ---------------------- */
template <typename CharT, typename Traits = std::char_traits<CharT>,
    typename Alloc = std::allocator<CharT> >
std::vector< std::basic_string<CharT, Traits, Alloc> >
parse_csv_line(const std::basic_string<CharT, Traits, Alloc>& line,
    CharT delim = make_ch<CharT>(','))     // default ','
{
    typedef std::basic_string<CharT, Traits, Alloc> string_type;
    std::vector<string_type> result;
    string_type field;

    std::size_t i = 0, n = line.size();
    const CharT quote = make_ch<CharT>('"');

    while (i < n)
    {
        field.clear();
        bool quoted = (line[i] == quote);
        if (quoted) ++i;                              // skip opening "

        for (; i < n; ++i)
        {
            CharT c = line[i];

            if (quoted)
            {
                if (c == quote)
                {
                    if (i + 1 < n && line[i + 1] == quote)
                    {                                 // "" → literal "
                        field += quote;
                        ++i;
                    }
                    else                             // closing "
                    {
                        ++i;
                        break;
                    }
                }
                else field += c;
            }
            else if (c == delim)                     // unquoted field end
            {
                ++i;
                break;
            }
            else field += c;
        }

        result.push_back(field);

        if (quoted && i < n && line[i] == delim) ++i; // skip delim after "
    }

    if (!line.empty() && line[line.size() - 1] == delim)
        result.push_back(string_type());             // trailing empty

    return result;
}

/* ---------- streaming reader ----------------------------------------- */
template <typename CharT, typename Traits = std::char_traits<CharT> >
class CsvReaderT
{
public:
    typedef std::basic_istream<CharT, Traits> istream_type;
    typedef std::basic_string<CharT, Traits>  string_type;
    typedef std::vector<string_type>         row_type;

    CsvReaderT(istream_type& in, CharT delim = make_ch<CharT>(',')) :
        m_in(in), m_delim(delim) {
    }

    bool next(row_type& row)
    {
        row.clear();
        string_type line;

        bool in_quotes = false;
        typename Traits::int_type ch_i;
        const CharT quote = make_ch<CharT>('"');
        const CharT lf = make_ch<CharT>('\n');
        const CharT cr = make_ch<CharT>('\r');

        while ((ch_i = m_in.get()) != Traits::eof())
        {
            CharT ch = Traits::to_char_type(ch_i);

            if (ch == cr) continue;          // normalize CRLF
            if (ch == quote) in_quotes = !in_quotes;
            if (ch == lf && !in_quotes) break;

            line += ch;
        }

        if (ch_i == Traits::eof() && line.empty())
            return false;                    // true EOF

        row = parse_csv_line(line, m_delim);
        return true;
    }

private:
    istream_type& m_in;
    CharT         m_delim;
};

/* convenient aliases */
typedef CsvReaderT<char>     CsvReader;
typedef CsvReaderT<wchar_t>  WCsvReader;

#endif /* CSV_READER_UNICODE_HPP_INCLUDED */
