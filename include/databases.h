#ifndef DATABASE_H
#define DATABASE_H
#include <vector>
#include "table.h"
#include "tuple.h"
#include <exception>

extern uint32_t block_size;

class All_db;
extern Database* db;

class Database 
{
protected:

    // -1 = custom database
    int8_t db_type = -1;
    std::string db_name = "";
    uint32_t bl_size = block_size;
    std::vector<Table*> tb_vec;
    // num of data blocks

public:

    Database () {};
    Database (int8_t type, std::string name = "") : db_type(type) 
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
            if (db_type == 2)
            {
                tb_vec.push_back(new Dis_table (6));
                tb_vec.push_back(new Borg_table);

            } else if (db_type != 1)
                throw std::invalid_argument("Unknown DB type");

        } else {
            db_name = "information_schema";
            tb_vec.push_back(new DB_table);
            tb_vec.push_back(new Tb_table);
        }

    }

    void insert (std::vector<std::string> l1, 
                 std::vector<uint32_t> l2, uint8_t tb_num)
    {
        if (tb_num > tb_vec.size())
            throw std::invalid_argument("Wrong number");

        tb_vec[tb_num]->insert(l1, l2);

        for (auto x : tb_vec)
        {
            // smth with tables_db
        }
    }

    void select (std::vector<uint32_t> tb_num, std::vector<uint8_t> col_names,
            int8_t num_of_col = -1, char sign = ' ', int32_t cond = -1)
    {
        /*
        if (num_of_col == -1 || sign == ' ' || cond == -1)
            tb_vec[tb_num]->select(col_num);
        else 
            tb_vec[tb_num]->select(col_num, num_of_col, sign, cond);
            */

        for (auto x : tb_num)
            if (x < tb_vec.size())
                tb_vec[x]->select(col_names, num_of_col, sign, cond);
            else 
                throw std::invalid_argument("Invalid column num");
    }
    
    void write () { db_full_write(*this); }
    // void read (std::string name) { this = db_meta_read(this, name); }

    friend void db_full_write (Database&);
    friend Database* db_meta_read (Database* db, std::string name);
    friend Database& init_tables(Database& db);

    ~Database ()
    {
        for (auto& x : tb_vec)
            delete x;
    }
};


/*
// Meta database
class All_db : public Database // schema 0 type
{
private:
    DB_table* db_t;
    Tb_table* tb_t;

public:

    All_db ()
    {
        db_type = 0;
        db_name = "information_schema";
        db_t = new DB_table;
        tb_t = new Tb_table;

        tb_vec = {db_t, tb_t};
    }

    ~All_db ()
    {
        delete db_t;
        delete tb_t;
    }
};

// Faculties
class Virtual_faculty : public Database
{
    Fac_table* fac_t;
    Dep_table* dep_t;
    Dis_table* dis_dep_t;

public:

    explicit Virtual_faculty (std::string name)
    {
        tb_vec = {fac_t, dep_t, dis_dep_t};
    }
};

// 1 type
class Common_faculty : public Virtual_faculty // schema 1 type
{

};

// 2 type
class Special_faculty : public Virtual_faculty // schema 2 type
{
public:
    //Borg_table borg_t;
    //Dis_table dis_borg_t;
    //std::vector<Table> tb_vec;
};
*/

#endif
