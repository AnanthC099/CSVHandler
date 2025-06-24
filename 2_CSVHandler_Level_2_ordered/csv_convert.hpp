#ifndef CSV_CONVERT_HPP_INCLUDED
#define CSV_CONVERT_HPP_INCLUDED
/* ------------------------------------------------------------- *
 *  Lightweight string → number/bool conversions (C++98)         *
 * ----------------------------------------------------------- */
#include <sstream>

template <typename Target, typename CharT>
inline bool convert(const std::basic_string<CharT>& s, Target& out)
{
    std::basic_istringstream<CharT> ss(s);
    ss >> out;
    return !ss.fail() && ss.eof();
}

/* bool specialisation: allow Yes/No, True/False, 1/0 */
template <typename CharT>
inline bool convert(const std::basic_string<CharT>& s, bool& out)
{
    if (s.empty()) return false;
    CharT c = s[0];
    if (c == '1' || c == 'T' || c == 't' || c == 'Y' || c == 'y')
    {
        out = true;  return true;
    }
    if (c == '0' || c == 'F' || c == 'f' || c == 'N' || c == 'n')
    {
        out = false; return true;
    }
    return false;
}
#endif /* CSV_CONVERT_HPP_INCLUDED */
