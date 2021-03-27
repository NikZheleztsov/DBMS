#include "databases.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
extern fs::path root;
extern uint32_t block_size;

void db_write (Database& db)
{
    std::string db_dir = ".databases";
    fs::path current = root/db_dir;
    
    if (fs::exists(current))
    {
        std::ofstream file (current/db.db_name, std::ios::binary | std::ios::out);
        if (file.is_open())
        {
            file.close();
        } else {
            std::cout << "Unable to create a database file\n";
            return;
        }

    } else {
        std::cout << "Unable to find directory with databases\nExiting ...\n";
        return;
    }
}

