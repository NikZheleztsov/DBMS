#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "databases.h"
#include "tree.h"

namespace fs = std::filesystem;
uint32_t block_size = 1024;
fs::path root;
std::string config_name = ".config",
            db_dir = ".databases";

extern Database* db;

void config (fs::path current)
{
    std::string config_name = ".config";
    std::ifstream config (current/config_name);
    if (config.is_open())
    {
        std::string config_str;
        std::getline(config, config_str, '\0');
        int bl_s = config_str.find("BLOCK_SIZE");
        int end = config_str.find('\n', bl_s);

        if (end == -1)
            end = config_str.find(EOF, bl_s);

        int eq = config_str.find('=', bl_s);

        if (end != -1 && eq != -1)
        {
            try {

                uint32_t bl_size = std::stoi(config_str.substr(eq + 2, end - eq - 2));
                if (bl_size > 1023)
                    block_size = bl_size;

            } catch (...) {
                std::cout << "Error while reading config file\n";
                return;
            }
        }

        config.close();

    } else {
        std::cout << "Error while opening config file\n";
        return;
    }
}

void check_for_init()
{
    fs::path current = fs::current_path();

    // find root
    if (fs::exists(current/config_name))
    {
        root = current;

    } else {

        current = current.remove_filename();

        if (fs::exists(current/config_name))
        {
            root = current;

        } else {

            std::cout << "Unable to find DBMS directory\n\
                          Try to put binary file in DBMS directory\n\
                          You also need to have .config file in it\n";
            return;
        }
    }

    if (!fs::exists(root/db_dir/"information_schema"))
    {
            std::cout << "Initial configuration. Please wait a second\n";
            config(root);

            db = new Database(0);
            db->insert({"information_schema"}, {0}, 0);
            db->insert({"db_table"}, {1, 0}, 1);
            db->insert({"tb_table"}, {2, 0}, 1);
            db->write();

    } else {
        // db->read("information_schema");
        db = db_meta_read(db, "information_schema");
        // reading of main db and push all names with id to AVL tree
    }
}


