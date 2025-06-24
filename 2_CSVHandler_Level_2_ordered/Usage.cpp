/* ------------------------------------------------------------------
 *  Zoom participant CSV post‑processor
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
#include <unordered_map>
#include <unordered_set>        //  ⬅  NEW
#include <algorithm>
#include <cctype>

 /* ---------- helpers --------------------------------------------- */
static bool icontains(const std::string& hay, const std::string& needle)
{
    if (needle.empty()) return true;
    for (std::size_t i = 0; i + needle.size() <= hay.size(); ++i)
    {
        std::size_t j = 0;
        while (j < needle.size() &&
            std::tolower(static_cast<unsigned char>(hay[i + j])) ==
            std::tolower(static_cast<unsigned char>(needle[j]))) ++j;
        if (j == needle.size()) return true;
    }
    return false;
}
static void rtrim(std::string& s)
{
    while (!s.empty() &&
        (s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();
}
static std::string join_semicolon(const std::vector<std::string>& v)
{
    std::ostringstream os;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i) os << "; ";
        os << v[i];
    }
    return os.str();
}

/* ---------- data model ------------------------------------------- */
struct Participant
{
    std::string name;
    std::string email;
    std::string join_time;
    std::string leave_time;
    int         duration = 0;
    bool        guest = false;
    bool        in_waiting_room = false;
};

/* ---------- main -------------------------------------------------- */
int main()
{
    /* 1. input ---------------------------------------------------- */
    const std::string input_path =
        "C:\\Users\\anant\\Documents\\2_CSVHandler_Level_1\\Docs\\participants_84784749718_2025_06_23.csv";

    std::ifstream fin(input_path, std::ios::binary);
    if (!fin) { std::cerr << "Cannot open '" << input_path << "'\n"; return 1; }
    skip_bom(fin);

    /* 2. extract participant section ----------------------------- */
    std::ostringstream buf;
    std::string line;  bool header_found = false;
    while (std::getline(fin, line))
    {
        rtrim(line);
        if (!header_found) {
            if (icontains(line, "name (original name") && icontains(line, "email"))
            {
                header_found = true; buf << line << '\n';
            }
        }
        else buf << line << '\n';
    }
    if (!header_found) { std::cerr << "Participants header not found\n"; return 2; }

    /* 3. parse ---------------------------------------------------- */
    std::istringstream part_stream(buf.str());
    CsvTable<char> tbl(part_stream);

    std::vector<Participant> rows; rows.reserve(tbl.row_count());
    using EmailRowMap = std::unordered_map<std::string, std::vector<std::size_t>>;
    EmailRowMap email_rows;

    for (std::size_t r = 0; r < tbl.row_count(); ++r)
    {
        Participant p;
        p.name = tbl("Name (original name)", r);
        p.email = tbl("Email", r);
        p.join_time = tbl("Join time", r);
        p.leave_time = tbl("Leave time", r);
        convert(tbl("Duration (minutes)", r), p.duration);
        convert(tbl("Guest", r), p.guest);
        convert(tbl("In waiting room", r), p.in_waiting_room);

        rows.push_back(p);
        email_rows[p.email].push_back(rows.size() - 1);
    }

    /* 4. name -> UNIQUE e‑mail list ------------------------------ */
    std::unordered_map<std::string, std::vector<std::string>> name_to_emails;
    std::unordered_map<std::string, std::unordered_set<std::string>>  // CHANGED ⬅
        name_to_seen;                                                 // CHANGED ⬅

    for (const auto& kv : email_rows)
    {
        const std::string& email = kv.first;
        for (std::size_t idx : kv.second)
        {
            const std::string& name = rows[idx].name;
            auto& seen = name_to_seen[name];                          // CHANGED ⬅
            if (seen.insert(email).second)                            // CHANGED ⬅
                name_to_emails[name].push_back(email);                // CHANGED ⬅
        }
    }

    std::unordered_map<std::string, std::string> name_to_emails_joined;
    for (const auto& kv : name_to_emails)
        name_to_emails_joined[kv.first] = join_semicolon(kv.second);

    /* 5. ascending e‑mail order ---------------------------------- */
    std::vector<std::string> emails_sorted;
    emails_sorted.reserve(email_rows.size());
    for (const auto& kv : email_rows) emails_sorted.push_back(kv.first);
    std::sort(emails_sorted.begin(), emails_sorted.end());

    /* 6. output file --------------------------------------------- */
    std::ofstream fout("participants_grouped.csv", std::ios::binary);
    if (!fout) { std::cerr << "Cannot create participants_grouped.csv\n"; return 3; }

    const std::string header =
        "Name (Original Name),Emails (grouped),Join Time,Leave Time,"
        "Duration (Minutes),Guest,In Waiting Room";
    std::cout << header << '\n';
    fout << header << "\r\n";

    auto write_csv_line = [&](std::ostream& os,
        const Participant& p,
        const std::string& grouped_emails)
        {
            const bool to_file = (&os == &fout);
            const char* eol = to_file ? "\r\n" : "\n";
            os << p.name << ','
                << grouped_emails << ','
                << p.join_time << ','
                << p.leave_time << ','
                << p.duration << ','
                << (p.guest ? "Yes" : "No") << ','
                << (p.in_waiting_room ? "Yes" : "No") << eol;
        };

    /* 7. print/write --------------------------------------------- */
    for (const std::string& email : emails_sorted)
        for (std::size_t idx : email_rows[email]) {
            const Participant& p = rows[idx];
            write_csv_line(std::cout, p, name_to_emails_joined[p.name]);
            write_csv_line(fout, p, name_to_emails_joined[p.name]);
        }

    std::cout << "\nCreated participants_grouped.csv\n";
    return 0;
}
