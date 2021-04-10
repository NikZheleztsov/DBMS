#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "databases.h"
#include "fs_work.h"
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
std::vector<std::string> names;

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

            if (!fs::exists(root/db_dir))
                fs::create_directory(root/db_dir);

            db = new Database(0, "", true);
            db->insert({"information_schema"}, {0}, 0);
//            db->insert({"db_table"}, {1, 0}, 1);
//            db->insert({"tb_table"}, {2, 0}, 1);
            db->write();

    } else {
        db = db_meta_read("information_schema");
    }

    if (db != nullptr)
        for (auto x : db->tb_vec[0]->tuple_map)
            names.push_back(x.second->name);
}

void show_databases ()
{
    T_print tb;
    uint8_t max_w = 9;
    std::vector<std::string> names;

    for (auto x : db->tb_vec[0]->tuple_map)
    {
        names.push_back(x.second->name);
        if (x.second->name.size() > max_w)
            max_w = x.second->name.size();
    }

    tb.push_header({"databases"}, {max_w});
    for (auto x : names)
        tb.push_tuple({x});
    tb.print();
}

void show_databases_out (bool is_first = true)
{
    if (is_first)
    {
        if (db != nullptr)
            show_databases();
        else {
            std::cout << "Error while accessing SCHEMATA database\n\
                Trying to open it ...\n";
            check_for_init();
            show_databases_out(false);
        }
    } else {
        if (db != nullptr)
            show_databases();
        else 
            throw;
    }
}

void show_tables ()
{
    if (current_db != nullptr)
    {
        T_print tb;
        uint8_t max_w = 6;
        std::vector<std::string> names;

        for (auto x : current_db->tb_vec)
        {
            names.push_back(x->table_name);
            if (x->table_name.size() > max_w)
                max_w = x->table_name.size();
        }

        tb.push_header({"tables"}, {max_w});
        for (auto x : names)
            tb.push_tuple({x});
        tb.print();

    } else 
        std::cout << "No database is used\n";
}

void describe_table (std::string name)
{
    if (current_db == nullptr)
    {
        std::cout << "No database is used\n";
        return;
    }

    bool is_ok = false;
    int i = 0;
    for (auto x : current_db->tb_vec)
    {
        i++;
        if (x->table_name == name)
        {
            is_ok = true;
            break;
        }
    }

    i--;

    if (is_ok && i < current_db->tb_vec.size())
    {
        T_print tb;
        uint8_t max_w = name.size() + 8;
        std::vector<std::string> names;

        for (auto x : current_db->tb_vec[i]->col_names)
        {
            names.push_back(x);
            if (x.size() > max_w)
                max_w = x.size();
        }

        // don`t output id
        names.erase(names.begin());

        tb.push_header({name + " columns"}, {max_w});
        for (auto x : names)
            tb.push_tuple({x});

        tb.print();

    } else
        std::cout << "Unknown table name\n";
}

void help () 
{
    std::cout << "This is some kind of documentation\n";
}

void case_sens (std::string& str)
{
    for (int i = 0; i < str.size(); i++)
    {
        char& y = str[i];
        if (y >= 'A' && y <= 'Z')
            y += 32;
    }
}

void parsing (std::vector<std::string> w);

void parsing_in (std::vector<std::string> all_words)
{
    if (all_words.size() == 2)
    {
        if (all_words[0] == "show")
        {
            case_sens(all_words[1]);

            if (all_words[1] == "databases")
            {
                show_databases_out();

            } else if (all_words[1] == "tables")
            {
                show_tables();
            } else 
                std::cout << "Unknown command\n";        

        } else if (all_words[0] == "use")
        {
            bool is_ok = false;
            for (auto x : names)
                if (x == all_words[1])
                {
                    is_ok = true;
                    break;
                }

            if (is_ok)
            {
                try {
                    if (current_db != nullptr)
                    {
                        Database* temp = current_db;
                        db_full_write(*current_db);
                        delete temp;
                        current_db = nullptr;
                    }

                    current_db = db_meta_read(all_words[1]);
                    curr_db = all_words[1];

                } catch (...) {
                    std::cout << "Error while writing/reading database\n";
                }

            } else
                std::cout << "Unknown name of database\n";

        } else if (all_words[0] == "describe")
        {
                describe_table(all_words[1]);
        } else
            std::cout << "Unknown command\n";

    } else if (all_words.size() == 3)
    {
        if (all_words[0] == "drop")
        {
            case_sens(all_words[1]);
            if (all_words[1] == "database")
            {
                if (fs::exists(root/db_dir/all_words[2]))
                {
                    if (all_words[2] == "information_schema")
                    {
                        std::cout << "Forbidden to delete this database\n";
                        return;
                    }

                    fs::remove(root/db_dir/all_words[2]);
                    uint32_t id = db->get_id(0, all_words[2]);
                    db->delete_id (0, id);
                    for (auto x = names.begin(); x < names.end(); ++x)
                        if (*x == all_words[2])
                        {
                            names.erase(x);
                            break;
                        }

                    if (curr_db == all_words[2])
                    {
                        curr_db = "none";
                        delete current_db;
                        current_db = nullptr;
                    }

                } else 
                    std::cout << "Unknown database name\n";
            } else
                std::cout << "Unknown command\n";
        } else 
            std::cout << "Unknown command\n";

    } else if (all_words.size() == 4)
    {
        if (all_words[0] == "create")
        {
            case_sens(all_words[1]);

            if ( all_words[1] == "database")
            {
                std::string name = all_words[2];
                case_sens(all_words[3]);
                // del all ' ' from (type = ...)
                all_words[3].erase(std::remove(all_words[3].begin(), 
                            all_words[3].end(), ' '), all_words[3].end());

                if (all_words[3].find("type", 0) != std::string::npos &&
                        all_words[3][0] == '(' && 
                        all_words[3][all_words[3].size() - 1] == ')')
                {
                    uint32_t pos_of_eq = all_words[3].find('=', 0);
                    std::string type = all_words[3].substr(pos_of_eq + 1, 
                            all_words[3].size() - pos_of_eq - 2);

                    if (type == "common")
                        Database new_db (1, name, true);
                    else if (type == "hybrid")
                        Database new_db (2, name, true);
                    else
                        std::cout << "Unknown database type\n";

                } else {
                    std::cout << "Unknown argument of database creation or wrong format\n";
                    return;
                }
            }
        } else if (all_words[0] == "insert")
        {
            case_sens(all_words[2]);
            if (all_words[2] == "values")
            {
                if (current_db == nullptr)
                {
                    std::cout << "No database is used\n";
                    return;
                }

                bool is_ok = false;
                int i = -1;
                for (auto x : current_db->tb_vec)
                {
                    i++;
                    if (x->table_name == all_words[1])
                    {
                        is_ok = true;
                        break;
                    }
                }

                if (is_ok)
                {
                    std::string val = all_words[3];
                    if (val[0] == '(' && val[val.size() - 1] == ')')
                    {
                        std::vector<std::string> str_val;
                        std::vector<uint32_t> int_val;
                        std::vector<std::string> data_t;

                        // del all ' '
                        val.erase(std::remove(val.begin(), 
                                    val.end(), ' '), val.end());

                        // parsing of (...)
                        for (size_t pos = 0, prev_pos = 0; (pos = val.find(',', prev_pos + 1)) 
                                && prev_pos != -1; prev_pos = pos)
                        {
                            std::string temp;
                            uint32_t tr;
                            prev_pos++;
                            if (pos != std::string::npos)
                                temp = val.substr(prev_pos, pos - prev_pos);
                            else 
                                temp = val.substr(prev_pos, val.size() - 1 - prev_pos);

                            if (temp[0] == '\'' && temp[temp.size() - 1] == '\'')
                            {
                                data_t.push_back("str");

                                std::stringstream in (temp);
                                std::string no_quo;
                                in >> std::quoted(no_quo, '\'');
                                str_val.push_back(no_quo);

                            } else {
                                try {
                                    tr = std::stoi(temp);
                                } catch (...) {
                                    std::cout << "Wrong value\n";
                                    return;
                                }

                                int_val.push_back(tr);
                                data_t.push_back("int");
                            }
                        }

                        if (data_t != current_db->tb_vec[i]->data_types)
                        {
                            std::cout << "Wrong value\n";
                            return;

                        } else {
                            current_db->insert(str_val, int_val, i);
                        }

                    } else 
                        std::cout << "Wrong value format (e.g. \"('str', 5)\")\n";
                } else 
                    std::cout << "Unknown table name";
            } else
                std::cout << "Unknown command\n";
        } else 
            std::cout << "Unknown command\n";

    } else if (all_words.size() == 5)
    {
        if (all_words[0] == "insert")
        {
            // deleting of 'into'
            std::string temp_2 = all_words[1];
            case_sens(temp_2);
            if (temp_2 == "into")
            {
                all_words.erase(++all_words.begin());
                parsing(all_words);

            } else {
                std::cout << "Unknown command\n";
            }
        } else if (all_words[0] == "delete")
        {
            case_sens(all_words[1]);
            case_sens(all_words[3]);
            if (all_words[1] == "from" && all_words[3] == "where")
            {
                // check if table exists
                if (current_db == nullptr)
                {
                    std::cout << "No database is used\n";
                    return;
                }

                bool is_ok = false;
                int i = -1;
                for (auto x : current_db->tb_vec)
                {
                    i++;
                    if (x->table_name == all_words[2])
                    {
                        is_ok = true;
                        break;
                    }
                }

                if (is_ok)
                {
                    // parsing of argument
                    size_t eq_pos = all_words[4].find('=');
                    if (eq_pos != std::string::npos)
                    {
                        std::string arg = all_words[4].substr(eq_pos + 1, all_words[4].size() - eq_pos - 1);
                        int id = -1;
                        if (all_words[4].find("id") != std::string::npos)
                        {
                            try {
                                id = std::stoi(arg);
                            } catch (...) {
                                std::cout << "Wrong id format\n";
                                return;
                            }

                            if (id > current_db->tb_vec[i]->tuple_map.size() - 1)
                                id = -1;

                        } else if (all_words[4].find("name") != std::string::npos)
                        {
                            std::stringstream in (arg);
                            std::string no_quo;
                            in >> std::quoted(no_quo, '\'');
                            id = current_db->get_id(i, no_quo);

                        } else { 
                            std::cout << "Unknown delete argument\n";
                            return;
                        }

                        if (id != -1)
                        {
                            current_db->delete_id(i, id);

                        } else 
                            std::cout << "No tuple with " << all_words[4] << " was found\n";
                    } else 
                        std::cout << "Wrong delete argument format\n";
                } else 
                    std::cout << "Unknown table name\n";
            } else 
                std::cout << "Unknown command\n";

        } else if (all_words[0] == "rename")
        {
            case_sens(all_words[1]);
            case_sens(all_words[3]);
            if (all_words[1] == "database" && all_words[3] == "to")
            {
                bool is_ok = false;
                for (auto x : names)
                    if (x == all_words[2])
                        is_ok = true;

                if (all_words[2] == "information_schema")
                {
                    std::cout << "Forbidden to rename this database\n";
                    return;
                }

                if (is_ok)
                {
                    if (current_db == nullptr ? 1 : current_db->db_name != all_words[2])
                    {
                        Database* temp_db;
                        temp_db = db_meta_read(all_words[2]);
                        temp_db->rename(all_words[4]); // add not full writing 
                        delete temp_db;
                    } else {
                        current_db->rename(all_words[4]);
                        curr_db = all_words[4];
                    }

                } else 
                    std::cout << "Unknown database name\n";
            } else 
                std::cout << "Unknown command\n";
        } else 
            std::cout << "Unknown command\n";
    } else 
        std::cout << "Unknown command\n";
}

void parsing (std::vector<std::string> w) // for inside use only
{
    parsing_in (w);
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
    for (int pos = 0, prev_pos = 0; 
            (pos = answ.find(' ', pos + 1)) && prev_pos != -1; prev_pos = pos)
    {
        if (prev_pos != 0)
            prev_pos++;

        std::string temp = answ.substr(prev_pos, pos - prev_pos);

        // all ( ... ) to one word
        if (!temp.empty())
        {
            if (temp[0] == '(')
            {
                if (pos != -1)
                    temp += answ.substr(pos + 1);
                pos = -1;
            }
        }

        all_words.push_back(temp);
    }

    // delete ';' in the last word
    std::string* str_pointer = &all_words[all_words.size() - 1];
    str_pointer->erase(str_pointer->size() - 1, 1);

    // case insensitivity of first word
    case_sens(all_words[0]);

    /*
    for (auto x : all_words)
        std::cout << x << std::endl;
        */

    parsing_in (all_words);
}
