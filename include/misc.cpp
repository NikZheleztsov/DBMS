#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "databases.h"
#include "tree.h"
#include "t_print.h"
#include <algorithm>

namespace fs = std::filesystem;
uint32_t block_size = 1024;
fs::path root;
std::string config_name = ".config",
            db_dir = ".databases";

extern Database* db;
extern Database* current_db;
extern std::string curr_db;

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
//            db->insert({"db_table"}, {1, 0}, 1);
//            db->insert({"tb_table"}, {2, 0}, 1);
            db->write();

    } else {
        // db->read("information_schema");
        db = db_meta_read(db, "information_schema");
        // reading of main db and push all names with id to AVL tree
    }
}

void show_databases ()
{
    T_print tb;
    uint8_t max_w = 0;
    std::vector<std::string> names;

    for (auto x : db->tb_vec[0]->tuple_map)
    {
        names.push_back(x.second->name);
        if (x.second->name.size() > max_w)
            max_w = x.second->name.size();
    }

    tb.push_header({"databases"}, {max_w});
    tb.push_tuple(names);
    tb.print();
}

void help () 
{
    std::cout << "This is some kind of documentation\n";
}

void parsing (std::string& answ)
{
    //remove duplicate/first/last spaces
    if (answ[0] == ' ')
        answ.erase(0, 1);

    for (int pos = 0; (pos = answ.find(' ', pos + 1)) != -1; )
    {
        while(answ[pos + 1] == ' ')
            answ.erase(pos + 1, 1);
    }

    if (answ[answ.size() - 2] == ' ')
        answ.erase(answ.size() - 2, 1);

    // all words to vec
    std::vector<std::string> all_words;
    for (int pos = 0, prev_pos = 0; (pos = answ.find(' ', pos + 1)) && prev_pos != -1; prev_pos = pos)
    {
        if (prev_pos != 0)
            prev_pos++;

        std::string temp = answ.substr(prev_pos, pos - prev_pos);

        // all ( ... ) to one word
        if (!temp.empty())
        {
            if (temp[0] == '(')
            {
                temp += answ.substr(pos + 1);
                pos = -1;
            }
        }

        all_words.push_back(temp);
    }

    // delete ';' in the last word
    std::string* str_pointer = &all_words[all_words.size() - 1];
    str_pointer->erase(str_pointer->size() - 1, 1);

    /*
    for (auto x : all_words)
        std::cout << x << std::endl;
        */

    if (all_words.size() > 1)
    {
        if (all_words.size() == 2)
        {
            if (all_words[0] == "show" || all_words[0] == "SHOW")
            {
                if (all_words[1] == "databases" || all_words[1] == "DATABASES")
                {
                    show_databases();

                } else if (all_words[1] == "tables" || all_words[1] == "TABLES")
                {
                } else 
                    std::cout << "Unknown command\n";

            } else if (all_words[0] == "describe" || all_words[0] == "DESCRIBE")
            {
                // if all_words[1] is a part of col_names of current db
            }
        
        } else if (all_words.size() == 3)
        {
            if ((all_words[0] == "use" || all_words[0] == "USE") &&
                    (all_words[1] == "database" || all_words[1] == "DATABASE"))
            {
            }

        } else if (all_words.size() == 4)
        {

        } else 
            std::cout << "Unknown command\n";

    } else 
        //one-word command 
        std::cout << "Unknown command\n";
}
