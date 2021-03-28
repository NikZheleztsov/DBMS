#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
uint32_t block_size = 1024;
fs::path root;
std::string config_name = ".config",
            db_dir = ".databases";

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
                block_size = std::stoi(config_str.substr(eq + 2, end - eq - 2));
            } catch (...) {
                std::cout << "Error while opening config file\n";
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

    if (!fs::exists(root/db_dir) || fs::is_empty(root/db_dir))
    {
            std::cout << "Initial configuration. Please wait a second\n";
            config(root);

            // initialization of DB_all database
    } 
}
