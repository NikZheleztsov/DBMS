#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
uint32_t block_size;
fs::path root;

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

        std::cout << "Success\n";
        config.close();

    } else {
        std::cout << "Error while opening config file\n";
        return;
    }
}

void read_config () 
{
    std::string config_name = ".config";
    fs::path current = fs::current_path();

    if (fs::exists(current/config_name))
    {
        std::cout << "Reading config ... \n";
        root = current;
        config(current);

    } else {

        current = current.remove_filename();

        if (fs::exists(current/config_name))
        {
            std::cout << "Reading config ... \n";
            root = current;
            config(current);

        } else {

            std::cout << "Unable to open config file\n";
            return;
        }
    }
}
