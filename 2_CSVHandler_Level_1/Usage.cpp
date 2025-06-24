/* ------------------------------------------------------------------
 *  Example program:
 *  • Extracts the participant table from a Zoom CSV export.
 *  • Maps each row to a Participant struct.
 *  • Prints all rows back out.
 * ----------------------------------------------------------------*/
#include "csv_reader_unicode.hpp"
#include "csv_bom.hpp"
#include "csv_table.hpp"
#include "csv_convert.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

 /* ---------- helper: ASCII case‑insensitive contains -------------- */
static bool icontains(const std::string& hay, const std::string& needle)
{
    if (needle.empty()) return true;
    for (std::size_t i = 0; i + needle.size() <= hay.size(); ++i)
    {
        std::size_t j = 0;
        while (j < needle.size() &&
            std::tolower(hay[i + j]) == std::tolower(needle[j])) ++j;
        if (j == needle.size()) return true;
    }
    return false;
}

/* ---------- data model ------------------------------------------- */
struct Participant
{
    std::string name;
    std::string email;
    std::string join_time;
    std::string leave_time;
    int         duration;
    bool        guest;
    bool        in_waiting_room;
};

/* ---------- main -------------------------------------------------- */
int main()
{
    std::ifstream fin("C:\\Users\\anant\\Documents\\2_CSVHandler_Level_1\\Docs\\participants_84784749718_2025_06_23.csv", std::ios::binary);
    if (!fin) { std::cerr << "Cannot open zoom_meeting.csv\n"; return 1; }
    skip_bom(fin);

    /* ---- copy participant section into memory ---- */
    std::ostringstream buf;
    std::string line;
    bool header = false;

    while (std::getline(fin, line))
    {
        if (!header)
        {
            if (icontains(line, "name (original name") && icontains(line, "email"))
            {
                header = true; buf << line << '\n';
            }
        }
        else buf << line << '\n';
    }
    if (!header)
    {
        std::cerr << "Participants header not found.\n";
        return 2;
    }

    /* ---- parse with CsvTable ---- */
    std::istringstream part_stream(buf.str());
    CsvTable<char> tbl(part_stream);

    /* ---- convert to structs ---- */
    std::vector<Participant> vec;
    vec.reserve(tbl.row_count());

    for (std::size_t r = 0; r < tbl.row_count(); ++r)
    {
        Participant p;
        p.name = tbl("Name (original name)", r);
        p.email = tbl("Email", r);
        p.join_time = tbl("Join time", r);
        p.leave_time = tbl("Leave time", r);

        if (!convert(tbl("Duration (minutes)", r), p.duration))
            p.duration = 0;

        if (!convert(tbl("Guest", r), p.guest))
            p.guest = false;
        if (!convert(tbl("In waiting room", r), p.in_waiting_room))
            p.in_waiting_room = false;

        vec.push_back(p);
    }

    /* ---- print all data ---- */
    std::cout << "Name,Email,Join time,Leave time,Duration (minutes),"
        "Guest,In waiting room\n";
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        const Participant& p = vec[i];
        std::cout << p.name << ',' << p.email << ',' << p.join_time << ','
            << p.leave_time << ',' << p.duration << ','
            << (p.guest ? "Yes" : "No") << ','
            << (p.in_waiting_room ? "Yes" : "No") << '\n';
    }
    return 0;
}
