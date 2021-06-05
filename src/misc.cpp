#include <string>
#include <set>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "databases.h"
#include "fs_work.h"
#include "t_print.h"
#include <algorithm>
#include <boost/range/combine.hpp>
#include <initializer_list>

namespace fs = std::filesystem;
uint32_t block_size = 1024;
fs::path root;
std::string config_name = ".config",
            db_dir = ".databases",
            scr_dir = "scripts";

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
        std::vector<std::string> data_types = {"int"};

        for (auto x : current_db->tb_vec[i]->col_names)
        {
            names.push_back(x);
            if (x.size() > max_w)
                max_w = x.size();
        }

        for (auto x : current_db->tb_vec[i]->data_types)
        {
            data_types.push_back(x);
        }

        tb.push_header({"Columns", "Types"}, {max_w, 5});
        for (auto x : boost::combine(names, data_types))
        {
            std::string n, t;
            boost::tie(n, t) = x;
            tb.push_tuple({n, t});
        }

        tb.print();

    } else
        std::cout << "Unknown table name\n";
}

void help () 
{

    {
        T_print tb;
        tb.push_header({"DBMS Guide"}, {52});
        tb.push_tuple({" DBMS - special-purpose (universities) database"});
        tb.push_tuple({" management system. The work is still in progress."});
        tb.push_tuple({" "});
        tb.push_tuple({" All commands are case insensitive (but not names!)"});
        tb.push_tuple({" Commands are also have to end with semicolon (';')"});
        tb.push_tuple({" Here are several guide aliases for custom values:"});
        tb.push_tuple({" <db> - database name, <tb> - table name, <cl> - "});
        tb.push_tuple({" column name, <val> - value, <sgn> - sign (=|>|<)."});
        tb.print();
    }

    std::cout << std::endl;

    {
        T_print tb;
        tb.push_header({"Commands", "Description"}, {21, 28});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" SHOW DATABASES"," -> list all databases"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" USE <db>", " -> use the <db> as the"});
        tb.push_tuple({" ", "    current database for"});
        tb.push_tuple({" ", "    subsequent commands"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" SHOW TABLES", " -> list all tables in the"});
        tb.push_tuple({" ", "    current database"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" DESCRIBE <tb>", " -> provide info about"});
        tb.push_tuple({" ", "    columns in a table"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" CREATE DATABASE <db>", " -> create a database"});
        tb.push_tuple({" (type=hybrid(common))", "    with a given name"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" DROP DATABASE <db>", " -> delete the database"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" RENAME DATABASE <db>"," -> change a database's"});
        tb.push_tuple({" TO <new_db>", "    name"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" INSERT <tb> VALUES ", " -> insert values into"});
        tb.push_tuple({" (<val>, ...)", "    table. All string have to"});
        tb.push_tuple({" ", "    be quoted. You don't have"});
        tb.push_tuple({" ", "    to insert primary id"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" DELETE FROM <tb>", " -> delete tuple with a"});
        tb.push_tuple({" WHERE id(name)=<val>", "    given id/name"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" UPDATE <tb> set", " -> replace chosen columns"});
        tb.push_tuple({" <cl>=<val>, ... ", "    with new values in a row"});
        tb.push_tuple({" WHERE id(name)=<val>", "    where id/name = <val>"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" SELECT <cl>, ... (*)", " -> retrive rows selected"});
        tb.push_tuple({" FROM <tb> [WHERE <cl>", "    from one table. See"});
        tb.push_tuple({" <sgn><val> ORDER BY", "    example for more info"});
        tb.push_tuple({" <cl> ASC(DESC) WRITE]", ""});
        tb.push_tuple({" ", " "});
        tb.print();

    }
    
    std::cout << std::endl << "Need to be replaced with JOIN & COUNT():\n";

    {
        T_print tb;
        tb.push_header({"Commands", "Description"}, {21, 28});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" disciplines_num <val>", " -> provide a number of"});
        tb.push_tuple({" ", "    disciplines taught at the"});
        tb.push_tuple({" ", "    faculty name of which "});
        tb.push_tuple({" ", "    = <val>"});
        tb.push_tuple({" ", " "});
        tb.push_tuple({" which_faculty <val>", " -> provide a name of"});
        tb.push_tuple({" ", "    faculty where discipline "});
        tb.push_tuple({" ", "    with a <val> name is "});
        tb.push_tuple({" ", "    being taught"});
        tb.print();
    }

    std::cout << std::endl << "Examples of command use: \n\n";
    std::cout << "DBMS [(MGTU)]> " << "CREATE DATABASE MGTU (type=hybrid);" << std::endl;
    std::cout << "DBMS [(MGTU)]> " << "INSERT faculties VALUES ('Special Machinery', 'NPO Machinery', 17, 1);" << std::endl;
    std::cout << "DBMS [(MGTU)]> " << "UPDATE faculties SET fac_name='New name', nuc_name='New NUC' WHERE id=0;" << std::endl;
    std::cout << "DBMS [(MGTU)]> " << "SELECT * FROM faculties;" << std::endl;
    std::cout << "DBMS [(MGTU)]> " << "SELECT fac_id, fac_name FROM faculties \
WHERE num_dep>7 ORDER BY nuc_name ASC;" << std::endl << std::endl;

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

int parsing (std::vector<std::string> w);
int parsing (std::string answ);

int parsing_in (std::vector<std::string> all_words)
{
    // replace all with lambda
    auto check_column = [] (uint8_t table_num, std::string name)->int8_t
    {
        int8_t where_col_num = 0;
        for (auto x : current_db->tb_vec[table_num]->col_names)
        {
            if (x == name)
                break;
            where_col_num++;
        }

        if (where_col_num == current_db->tb_vec[table_num]->col_names.size())
            return -1;
        else 
            return where_col_num;
    };

    auto check_table = [] (std::string name)->int8_t
    {
        int8_t where_tb_num = 0;
        for (auto x : current_db->tb_vec)
        {
            if (x->table_name == name)
                break;
            where_tb_num++;
        }

        if (where_tb_num == current_db->tb_vec.size())
            return -1;
        else 
            return where_tb_num;
    };

    if (all_words.size() == 2)
    {
        if (all_words[0] == "source")
        {
            // parsing of script
            std::cout << "Reading script...\n";
            std::ifstream script (root/scr_dir/all_words[1]);
            if (script.is_open())
            {
                std::string commands;
                std::getline (script, commands, '\0');
                
                // delete comments 
                while (commands.find('#') != std::string::npos)
                {
                    auto h = commands.find('#');
                    commands.erase(h, commands.find('\n', h) - h);
                }

                // delete \n
                for (auto n = commands.find('\n', 0); n != std::string::npos; n = commands.find('\n', n))
                    commands.erase(n, 1);

                size_t pos = 0; int ret = 0;
                auto end = commands.find(';', pos);
                while (end != std::string::npos)
                {
                    ret = parsing(commands.substr(pos, end - pos + 1));
                    if (ret == -1)
                    {
                        std::cout << "Error: " << commands.substr(pos, end - pos +1) << std::endl;
                        return -1;
                    }

                    pos = end + 1;
                    end = commands.find(';', pos);
                }

                std::cout << "Success!\n";

            } else {
                std::cout << "Unable to open a file. Aborting...\n";
                return -1;
            }

        } else if (all_words[0] == "show")
        {
            case_sens(all_words[1]);

            if (all_words[1] == "databases")
            {
                show_databases_out();

            } else if (all_words[1] == "tables")
            {
                show_tables();
            } else {
                std::cout << "Unknown command\n";        
                return -1;
            }

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
                        current_db->write();
                        delete current_db;
                    }

                    current_db = db_meta_read(all_words[1]);
                    curr_db = all_words[1];

                } catch (...) {
                    std::cout << "Error while writing/reading database\n";
                    return -1;
                }

            } else {
                std::cout << "Unknown name of database\n";
                return -1;
            }

        } else if (all_words[0] == "describe")
        {
                describe_table(all_words[1]);

        } else if (all_words[0] == "disciplines_num") //should be done with JOIN & COUNT (...)
        {
            if (!(current_db == nullptr && current_db->db_type == 0))
            {

                std::stringstream in (all_words[1]);
                in >> std::quoted(all_words[1], '\'');

                auto id = current_db->get_id(0, all_words[1]);
                if (id == -1)
                {
                    std::cout << "Unable to find faculty\n";
                    return -1;
                }

                std::vector<uint32_t> dep_keys;

                for (auto x : current_db->tb_vec[1]->tuple_map)
                    if (x.second->for_key == id)
                        dep_keys.push_back(x.first);

                int dis_num = 0;
                for (auto x : current_db->tb_vec[2]->tuple_map)
                    for (auto y : dep_keys)
                        if (x.second->for_key == y)
                            dis_num++;

                if (current_db->db_type == 2)
                {
                    std::vector<uint32_t> borg_keys;
                    for (auto x : current_db->tb_vec[3]->tuple_map)
                        if (x.second->for_key == id)
                            borg_keys.push_back(x.first);

                    for (auto x : current_db->tb_vec[4]->tuple_map)
                        for (auto y : borg_keys)
                            if (x.second->for_key == y)
                                dis_num++;
                }

                std::cout << "Number of disciplines on " << 
                    all_words[1] << " -> " << dis_num << std::endl;

            } else {
                std::cout << "Wrong database is used\n";
                return -1;
            }

        } else if (all_words[0] == "which_faculty")
        {

            std::stringstream in (all_words[1]);
            in >> std::quoted(all_words[1], '\'');

            auto id_dep = current_db->get_id(2, all_words[1]);
            int32_t id_borg = -1;

            if (id_dep == -1 && current_db->db_type == 2)
                id_borg = current_db->get_id(4, all_words[1]);

            if (id_dep == -1 && id_borg == -1)
            {
                std::cout << "Unable to find discipline\n";
                return -1;
            }
            
            if (id_dep != -1)
            {
                tuple* tup = current_db->get_tuple(2, id_dep);
                tuple* dep = current_db->get_tuple(1, tup->for_key);
                tuple* fac = current_db->get_tuple(0, dep->for_key);
                std::cout << "Name of faculty -> " << fac->name << std::endl;
            } else
            {
                tuple* tup = current_db->get_tuple(4, id_dep);
                tuple* borg = current_db->get_tuple(3, tup->for_key);
                tuple* fac = current_db->get_tuple(0, borg->for_key);
                std::cout << "Name of faculty -> " << fac->name << std::endl;
            }
            
        } else {
            std::cout << "Unknown command\n";
            return -1;
        }

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
                        return -1;
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

                } else  {
                    std::cout << "Unknown database name\n";
                    return -1;
                }
            } else {
                std::cout << "Unknown command\n";
                return -1;
            }
        } else if (all_words[0] == "create")
        {
            all_words.push_back("(type=common)");
            parsing(all_words);
        } else {
            return -1;
            std::cout << "Unknown command\n";
        }

    } else if (all_words.size() == 4)
    {
        if (all_words[0] == "create")
        {
            case_sens(all_words[1]);

            if ( all_words[1] == "database")
            {
                std::string name = all_words[2];

                // no same db's names
                for (auto x : db->tb_vec[0]->tuple_map)
                    if (x.second->name == all_words[2])
                    {
                        std::cout << all_words[2] << " is already exist\n";
                        return -1;
                    }

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
                    else {
                        std::cout << "Unknown database type\n";
                        return -1;
                    }

                } else {
                    std::cout << "Unknown argument of database creation or wrong format\n";
                    return -1;
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
                    return -1;
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

                        // del all ' ' but not inside quotes
                        // val.erase(std::remove(val.begin(), 
                        //            val.end(), ' '), val.end());

                        bool is_quote = false;
                        for (int i = 0; i < val.size(); i++)
                        {
                            if (val[i] == ' ')
                                if (!is_quote)
                                    val.erase(i, 1);

                            if (val[i] == '\'')
                                is_quote = !is_quote;
                        }

                        // parsing of (...)
                        for (size_t pos = 0, prev_pos = 0; 
                                (pos = val.find(',', prev_pos + 1)) 
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
                                    return -1;
                                }

                                int_val.push_back(tr);
                                data_t.push_back("int");
                            }
                        }

                        for (auto x : str_val)
                            if (x.size() > 32)
                            {
                                std::cout << "String length must be < 32\n";
                                return -1;
                            }

                        if (data_t != current_db->tb_vec[i]->data_types)
                        {
                            std::cout << "Wrong value\n";
                            return -1;

                        } else {
                            try {
                                current_db->insert(str_val, int_val, i);
                            } catch (...) {
                                std::cout << "Wrong foreign key\n";
                                return -1;
                            }
                        }

                    } else {
                        std::cout << "Wrong value format (e.g. \"('str', 5)\")\n";
                        return -1;
                    }

                } else {
                    std::cout << "Unknown table name\n";
                    return -1;
                }

            } else {
                std::cout << "Unknown command\n";
                return -1;
            }
        } 

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
                return -1;
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
                    return -1;
                }

                if (all_words[2] == "inforamtion_schema")
                {
                    std::cout << "It is frobidden to delete from this database\n";
                    return -1;
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
                        std::string arg = all_words[4].substr
                            (eq_pos + 1, all_words[4].size() - eq_pos - 1);
                        int id = -1;
                        if (all_words[4].find("id") != std::string::npos)
                        {
                            try {
                                id = std::stoi(arg);
                            } catch (...) {
                                std::cout << "Wrong id format\n";
                                return -1;
                            }

                        } else if (all_words[4].find("name") != std::string::npos)
                        {
                            std::stringstream in (arg);
                            std::string no_quo;
                            in >> std::quoted(no_quo, '\'');
                            id = current_db->get_id(i, no_quo);

                        } else { 
                            std::cout << "Unknown delete argument\n";
                            return -1;
                        }

                        if (id != -1)
                        {
                            current_db->delete_id(i, id);

                            // cascade delete
                            std::set<int> set_dep;
                            std::set<int> set_borg;
                            // for key, not for foreign id !
                            if (i == 0) // faculty
                            {
                                auto f = current_db->tb_vec[1]->tuple_map.find(id);
                                while (f != current_db->tb_vec[1]->tuple_map.end())
                                {
                                    current_db->delete_id(1, f->first);
                                    set_dep.insert(f->first);
                                }

                                if (current_db->db_type == 2)
                                {
                                    auto f = current_db->tb_vec[3]->tuple_map.find(id);
                                    while (f != current_db->tb_vec[3]->tuple_map.end())
                                    {
                                        current_db->delete_id(3, f->first);
                                        set_dep.insert(f->first);
                                        f = current_db->tb_vec[3]->tuple_map.find(id);
                                    }
                                }

                            } else if (i == 0 || i == 1 || i == 3) // dep or borg
                            {
                                if (i == 1)
                                    set_dep.insert(id);
                                else if (i == 3)
                                    set_borg.insert(id);

                                for (auto x : set_dep)
                                {
                                    auto f = current_db->tb_vec[2]->tuple_map.find(x);
                                    while (f != current_db->tb_vec[2]->tuple_map.end())
                                    {
                                        current_db->delete_id(2, f->first);
                                        f = current_db->tb_vec[2]->tuple_map.find(x);
                                    }
                                }

                                if (current_db->db_type == 2)
                                {
                                    for (auto x : set_borg)
                                    {
                                        auto f = current_db->tb_vec[4]->tuple_map.find(x);
                                        while (f != current_db->tb_vec[4]->tuple_map.end())
                                        {
                                            current_db->delete_id(4, f->first);
                                            f = current_db->tb_vec[4]->tuple_map.find(x);
                                        }
                                    }
                                }
                            }

                        } else {
                            std::cout << "No tuple with " << 
                                all_words[4] << " was found\n";
                            return -1;
                        }

                    } else {
                        std::cout << "Wrong delete argument format\n";
                        return -1;
                    }

                } else {
                    std::cout << "Unknown table name\n";
                    return -1;
                }

            } else {
                std::cout << "Unknown command\n";
                return -1;
            }

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
                    return -1;
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

                } else {
                    std::cout << "Unknown database name\n";
                    return -1;
                }
            } else {
                std::cout << "Unknown command\n";
                return -1;
            }
        } else {
            std::cout << "Unknown command\n";
            return -1;
        }
    }

    if (all_words.size() > 3 && all_words[0] == "select")
    {
        if (current_db == nullptr)
        {
            std::cout << "No database is used\n";
            return -1;
        }

        int num_of_from = 1;
        for (auto str = all_words.begin() + 1; str != all_words.end(); ++str)
        {
            std::string temp = *str;
            case_sens(temp);
            if (temp == "from")
                break;
            num_of_from++;
        }

        // check of table
        bool is_ok = false;
        // !!!
        int table_num = 0;
        for (auto x : current_db->tb_vec)
        {
            if (x->table_name == all_words[num_of_from + 1])
            {
                is_ok = true;
                break;
            }

            table_num++;
        }

        if (!is_ok)
        {
            std::cout << "Unknown table name\n";
            return -1;
        }

        // !!!
        std::vector<int8_t> col_out;

        if (all_words[1] == "*")
            col_out.push_back(-1);
        else {
            for (auto str = all_words.begin() + 1; 
                    str != all_words.begin() + num_of_from; ++str)
            {
                int num_of_col = 0;
                for (auto x : current_db->tb_vec[table_num]->col_names)
                {
                    if (x == *str)
                        break;
                    num_of_col++;
                }

                if (num_of_col == current_db->tb_vec[table_num]->col_names.size())
                {
                    std::cout << "Unknown column name\n";
                    return -1;
                }

                col_out.push_back(num_of_col);
            }
        }

        // !!!
        int8_t where_col_num = -1;
        char sign = ' ';
        std::string cond = "";

        // !!!
        bool is_sort = false, dir = false, is_write=false;
        int8_t order_col = -1;

        if (all_words.size() > num_of_from + 2)
        {
            case_sens(all_words[num_of_from + 2]);
            int num_of_end = num_of_from + 1;

            // parsing of where
            if (all_words[num_of_end + 1] == "where")
            {
                if (all_words.size() < num_of_end + 3)
                {
                    std::cout << "Unknown command\n";
                    return -1;
                }

                // find any sign
                auto pos = std::string::npos;
                char signs [] = {'=', '>', '<'};
                int i = 0;
                for (; i < 3; i++)
                    if (pos == std::string::npos)
                        pos = all_words[num_of_end + 2].find(signs[i]);
                    else
                        break;
                i--;

                if (pos == std::string::npos)
                {
                    std::cout << "Unknown sign\n";
                    return -1;
                }

                sign = signs[i];
                auto col_name = all_words[num_of_end + 2].substr(0, pos);

                where_col_num = check_column(table_num, col_name);
                if (where_col_num == current_db->tb_vec[table_num]->col_names.size())
                {
                    std::cout << "Unknown column name\n";
                    return -1;
                }

                cond = all_words[num_of_end + 2].substr(pos + 1);

                // unquote
                std::stringstream in (cond);
                in >> std::quoted(cond, '\'');

                num_of_end +=2;
            }

            if (all_words.size() > num_of_end + 1)
            {
                if (all_words[num_of_end + 1] == "order")
                {
                    if (all_words.size() < num_of_end + 5)
                    {
                        std::cout << "Unknown command\n";
                        return -1;
                    }

                    is_sort = true;
                    case_sens(all_words[num_of_end + 2]);
                    case_sens(all_words[num_of_end + 4]);

                    if (all_words[num_of_end + 2] == "by")
                    {
                        order_col = check_column(table_num, all_words[num_of_end + 3]);
                        if (order_col == -1)
                        {
                            std::cout << "Unknown column name\n";
                            return -1;
                        }

                        dir = (all_words[num_of_end + 4] == "asc");
                    }

                    num_of_end +=4;
                }

                if (all_words.size() > num_of_end + 1)
                {
                    case_sens(all_words[num_of_end + 1]);
                    if (all_words[num_of_end + 1] == "write")
                        is_write = true;
                    else {
                        std::cout << "Unknown command \n";
                        return -1;
                    }
                }
            }
        }

        try {
            current_db->select(table_num,col_out, where_col_num, sign, cond,
                    is_sort, order_col, dir, is_write);
        } catch (...)
        {
            std::cout << "Error inside kernel\n";
            return -1;
        }

        // smth with files
    } else if (all_words.size() > 3 && all_words[0] == "update")
    {
        if (current_db == nullptr)
        {
            std::cout << "No database is used\n";
            return -1;
        }

        case_sens(all_words[2]);
        int table_num = check_table(all_words[1]);;
        if (all_words[2] != "set" && table_num  == -1)
        {
            std::cout << "Unknown command or table name\n";
            return -1;
        }
        
        int num_of_where = 3;
        for (int i = 3; i < all_words.size(); i++)
        {
            std::string temp = all_words[i];
            case_sens(temp);
            if (temp == "where")
                break;
            num_of_where++;
        }

        int8_t id = -1;
        // parsing of argument
        size_t eq_pos = all_words[num_of_where + 1].find('=');
        if (eq_pos != std::string::npos)
        {
            std::string arg = all_words[num_of_where + 1].substr
                (eq_pos + 1, all_words[num_of_where + 1].size() - eq_pos - 1);
            if (all_words[num_of_where + 1].find("id") != std::string::npos)
            {
                try {
                    id = std::stoi(arg);
                } catch (...) {
                    std::cout << "Wrong id format\n";
                    return -1;
                }

                if (id > current_db->tb_vec[table_num]->tuple_map.size() - 1)
                    id = -1;

            } else if (all_words[num_of_where + 1].find("name") 
                    != std::string::npos)
            {
                std::stringstream in (arg);
                std::string no_quo;
                in >> std::quoted(no_quo, '\'');
                id = current_db->get_id(table_num, no_quo);

            } else { 
                std::cout << "Unknown update argument\n";
                return -1;
            }
        }

        if (id != -1)
        {
            tuple* old_one = current_db->get_tuple(table_num, id);
            if (old_one == nullptr)
            {
                std::cout << "Error while getting tuple\n";
                return -1;
            }

            auto data = old_one->get_data();

            std::vector<std::string> l1 (data.first);
            std::vector<uint32_t> l2 (data.second);

            for (auto i = all_words.begin() + 3; 
                    i != all_words.begin() + num_of_where; i++)
            {
                int pos = i->find('=');
                std::string col_name = i->substr(0, pos);
                int col_num = check_column(table_num, col_name);
                if (col_num == -1)
                {
                    std::cout << "Unknown column name\n";
                    return -1;
                }

                std::string new_value = i->substr(pos + 1);
                if (new_value[0] == '\'')
                {
                    std::stringstream in (new_value);
                    in >> std::quoted(new_value, '\'');
                    if (current_db->tb_vec[table_num]->data_types[col_num - 1] != "str")
                    {
                        std::cout << "Wrong data format\n";
                        return -1;
                    }

                    l1[col_num - 1] = new_value;

                } else {

                    try {

                        if (current_db->tb_vec[table_num]->data_types[col_num - 1] != "int")
                        {
                            std::cout << "Wrong data format\n";
                            return -1;
                        }

                        l2[col_num - l1.size() - 1] = std::stoi(new_value);
                    } catch (...) {
                        std::cout << "Wrong new value format\n";
                        return -1;
                    }
                }

            }

            try {
                current_db->delete_id(table_num, id);
                current_db->insert(l1, l2, table_num, id);
            } catch (...) {
                std::cout << "Error while updating tuple\n";
                return -1;
            }

        } else {
            std::cout << "Wrong id\n";
            return -1;
        }
    } 

    return 0;
}

// for inside use only
int parsing (std::vector<std::string> w)
{
    return parsing_in (w);
}

int parsing (std::initializer_list<std::string> ilist)
{
    std::vector<std::string> vec (ilist);
    return parsing_in(vec);
}

int parsing (std::string answ)
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
                    temp += answ.substr(pos);
                pos = -1;

            } else if (pos != -1 && temp.find('\'') != std::string::npos)
            {
                int temp_pos = answ.find('\'', pos);
                temp += answ.substr(pos, temp_pos - pos + 1);
                pos = temp_pos + 1;

            } else {

                int pos_of_comma = 0;
                while ((pos_of_comma = temp.find(',')) != std::string::npos)
                    temp.erase(pos_of_comma);
            }
        }

        if (temp != "")
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

    return parsing_in (all_words);
}
