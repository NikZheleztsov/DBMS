#include <vector>
#include "table.h"
#include "tuple.h"
#include <exception>
#include <filesystem>
#include "databases.h"
#include <limits.h>

extern uint32_t block_size;
extern std::vector<std::string> names;
namespace fs = std::filesystem;

extern fs::path root;
extern std::string config_name,
       db_dir;

class All_db;
extern Database* db;

Database::Database (uint8_t type, std::string name, 
        bool is_first_create) : db_type(type) 
{
    // 0 = all_db
    // 1 = common faculty
    // 2 = hybrid faculty

    if (db_type)
    {
        if (name.empty())
            throw std::invalid_argument("Empty name");
        db_name = name;
        tb_vec.push_back(new Fac_table);
        tb_vec.push_back(new Dep_table);
        tb_vec.push_back(new Dis_table (5));

        if (is_first_create)
        {
            names.push_back(name);
            db->insert({name}, {type}, 0);
        }

        if (db_type == 2)
        {
            tb_vec.push_back(new Borg_table);
            tb_vec.push_back(new Dis_table (6));

        } else if (db_type != 1)
            throw std::invalid_argument("Unknown DB type");

    } else {
        db_name = "information_schema";
        tb_vec.push_back(new DB_table);
    }

    if (type != 0 && is_first_create)
        db_full_write(*this);
}

void Database::insert (std::vector<std::string> l1, 
        std::vector<uint32_t> l2, uint8_t tb_num, int32_t force_id)
{
    if (tb_num > tb_vec.size())
        throw std::invalid_argument("Wrong number");

    // WHY DOESNT EXCEPTION GO UPPER???
    // because can't catch empty exception (throw;)
   
    tb_vec[tb_num]->insert(l1, l2, force_id);
    write();
}

int32_t Database::get_id (uint8_t tb_num, std::string name)
{
    return tb_vec[tb_num]->get_id(name);
}

tuple* Database::get_tuple (uint8_t tb_num, uint32_t key)
{
    auto it = tb_vec[tb_num]->tuple_map.find(key);
    return (it != tb_vec[tb_num]->tuple_map.end()) ? 
        it->second : nullptr;
}

void Database::select (uint32_t tb_num, std::vector<int8_t> col_out, int8_t where_str, 
        char sign, std::string cond, 
        bool is_sort, int8_t col_num,  bool dir, bool is_write)
{
    if (tb_num < tb_vec.size())
        tb_vec[tb_num]->select(col_out, where_str, sign, cond, is_sort, col_num, dir, is_write);
    else 
        throw std::invalid_argument("Invalid column num");
}

int8_t Database::change_tuple (uint8_t tb_num, uint32_t key, tuple* new_tup)
{
    if (tb_vec[tb_num]->tuple_map.contains(key))
    {
        auto it = tb_vec[tb_num]->tuple_map.find(key);
        delete it->second;
        it->second = new_tup;
        write();
        return 0;
    } else 
        std::cout << "No such tuple in table\n";
    return 1;
}

void Database::rename (std::string new_name)
{
    std::string temp = db_name;
    int32_t temp_id = db->get_id(0, temp);

    // change in db name of database
    tuple* old_tup = db->get_tuple(0, temp_id);
    if (temp_id == -1 || old_tup == nullptr)
    {
        std::cout << "Error! Aborting...\n";
        return;
    }

    db_name = new_name;

    tuple* new_tup = new dbases 
        (dynamic_cast<dbases&>(*old_tup));
    new_tup->name = new_name;

    if (db->change_tuple(0, temp_id, new_tup))
    {
        std::cout << "Error! Aborting...\n";
        return;
    }

    // change in vec names
    for (auto x = names.begin(); x < names.end(); ++x)
        if (*x == temp)
        {
            *x = new_name;
            break;
        }

    this->write();
    if (fs::exists(root/db_dir/temp))
        fs::remove(root/db_dir/temp);
}

void Database::delete_id (uint8_t tb_num, uint32_t id)
{
    if (tb_num < tb_vec.size())
        tb_vec[tb_num]->delete_tuple(id);
    write();
}

void Database::write () { db_full_write(*this); }

Database::~Database ()
{
    for (auto& x : tb_vec)
        delete x;
}
