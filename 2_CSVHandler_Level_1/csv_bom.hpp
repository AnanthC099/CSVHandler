#ifndef CSV_BOM_HPP_INCLUDED
#define CSV_BOM_HPP_INCLUDED
/* ------------------------------------------------------------------ *
 *  Tiny utility: swallow UTF‑8 / UTF‑16 BOM at beginning of stream   *
 * ---------------------------------------------------------------- */
#include <istream>

template <typename CharT, typename Traits>
inline void skip_bom(std::basic_istream<CharT, Traits>& in)
{
    typename Traits::int_type c1 = in.peek();
    if (c1 == Traits::eof()) return;

    if (c1 == 0xEF)                       /* UTF‑8 BOM EF BB BF */
    {
        CharT b[3];
        if (in.read(b, 3) && b[1] == 0xBB && b[2] == 0xBF) return;
        in.seekg(0);
    }
    else if (c1 == 0xFE || c1 == 0xFF)    /* UTF‑16 BOM FF FE / FE FF */
    {
        CharT b[2];
        if (in.read(b, 2) &&
            ((b[0] == 0xFF && b[1] == 0xFE) ||
                (b[0] == 0xFE && b[1] == 0xFF))) return;
        in.seekg(0);
    }
}
#endif /* CSV_BOM_HPP_INCLUDED */
