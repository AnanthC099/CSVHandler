#include "CSVHandler.hpp"
#include <sstream>

using std::string;
using std::vector;

// ---------- helper ----------------------------------------------------------

vector<string> parse_csv_line(const string& line, char delim)
{
    vector<string> result;
    string field;
    std::size_t i = 0;
    const std::size_t n = line.size();

    while (i < n)
    {
        field.clear();
        bool quoted = (line[i] == '"');

        if (quoted) ++i;                           // skip opening quote

        for (; i < n; ++i)
        {
            char c = line[i];

            if (quoted)
            {
                if (c == '"')
                {
                    if (i + 1 < n && line[i + 1] == '"')
                    {                               // escaped quote
                        field += '"';
                        ++i;                        // skip second quote
                    }
                    else
                    {                               // end quote
                        ++i;
                        break;
                    }
                }
                else
                {
                    field += c;
                }
            }
            else if (c == delim)
            {
                ++i;
                break;                              // end of unquoted field
            }
            else
            {
                field += c;
            }
        }

        result.push_back(field);

        // skip delimiter after quoted field, if present
        if (quoted && i < n && line[i] == delim) ++i;
    }

    // If the last character is a delimiter, push an empty field.
    if (!line.empty() && line[line.size() - 1] == delim)
        result.push_back("");

    return result;
}

// ---------- CsvReader --------------------------------------------------------

bool CsvReader::next(vector<string>& row)
{
    row.clear();
    string line;

    // Read raw line—including embedded newlines in quoted fields.
    bool in_quotes = false;
    int  ch = 0;

    while ((ch = m_in.get()) != EOF)
    {
        if (ch == '\r')                             // unify CRLF -> LF
            continue;

        if (ch == '"')
            in_quotes = !in_quotes;

        if (ch == '\n' && !in_quotes)
            break;                                  // end of logical line

        line += static_cast<char>(ch);
    }

    if (ch == EOF && line.empty())
        return false;                               // true EOF

    row = parse_csv_line(line, m_delim);
    return true;
}
