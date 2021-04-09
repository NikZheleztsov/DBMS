#ifndef DATABASE_H
#define DATABASE_H
#include <vector>
#include "table.h"
#include "tuple.h"
#include <exception>

extern uint32_t block_size;
extern std::vector<std::string> names;

class All_db;
extern Database* db;

class Database 
{
protected:

    // -1 = custom database
    int8_t db_type = -1;
    std::string db_name = "";
    uint32_t bl_size = block_size;
    // num of data blocks

public:

    std::vector<Table*> tb_vec;

    Database () {};
    Database (uint8_t type, std::string name = "", bool is_first_create = false) : db_type(type) 
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
            // tb_vec.push_back(new Tb_table);
        }

        if (type != 0 && is_first_create)
            db_full_write(*this);
    }

    void insert (std::vector<std::string> l1, 
                 std::vector<uint32_t> l2, uint8_t tb_num)
    {
        if (tb_num > tb_vec.size())
            throw std::invalid_argument("Wrong number");

        tb_vec[tb_num]->insert(l1, l2);
        this->write();

    }

    void select (std::vector<uint32_t> tb_num, std::vector<uint8_t> col_names = {255},
            int32_t for_id = -1, int8_t num_of_col = -1, char sign = ' ', int64_t cond = -1)
    {
        /*
        if (num_of_col == -1 || sign == ' ' || cond == -1)
            tb_vec[tb_num]->select(col_num);
        else 
            tb_vec[tb_num]->select(col_num, num_of_col, sign, cond);
            */

        for (auto x : tb_num)
            if (x < tb_vec.size())
                tb_vec[x]->select(col_names, for_id, num_of_col, sign, cond);
            else 
                throw std::invalid_argument("Invalid column num");
    }

    uint32_t get_id (uint8_t tb_num, std::string name)
    {
        return tb_vec[tb_num]->get_id(name);
    }

    void delete_id (uint8_t tb_num, uint32_t id)
    {
        if (tb_num < tb_vec.size())
            tb_vec[tb_num]->delete_tuple(id);
        this->write();
    }
    
    void write () { db_full_write(*this); }

    friend void db_full_write (Database&);
    friend Database* db_meta_read (Database* db, std::string name);
    friend Database& init_tables(Database& db);

    ~Database ()
    {
        for (auto& x : tb_vec)
            delete x;
    }
};

#endif
