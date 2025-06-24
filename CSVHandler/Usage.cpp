#include "CSVHandler.hpp"
#include <fstream>
#include <iostream>

int main()
{
    std::ifstream fin("C:\\Users\\anant\\Documents\\CSVHandler\\Docs\\participants_84784749718_2025_06_23.csv");
    if (!fin)
    {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    CsvReader reader(fin);
    std::vector<std::string> row;

    while (reader.next(row))
    {
        for (std::size_t i = 0; i < row.size(); ++i)
        {
            std::cout << "[" << row[i] << "]";
            if (i + 1 < row.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }
    return 0;
}
