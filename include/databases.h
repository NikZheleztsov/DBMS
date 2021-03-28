#ifndef DATABASE_H
#define DATABASE_H
#include <vector>
#include "table.h"
#include "tuple.h"

extern uint32_t block_size;

class Database 
{
protected:

    std::string db_name;
    uint32_t bl_size = block_size;
    std::vector<Table*> tb_vec;
    // num of data blocks

public:
    virtual void insert (std::vector<std::string> l1,
                         std::vector<uint32_t> l2, uint8_t) = 0;
    virtual void write () = 0;
    virtual void read (std::string) = 0;

    friend void db_first_write (Database&);
    friend void db_read (Database& db, std::string name);
};

// Meta database
class All_db : public Database // schema 0 type
{
private:
    DB_table* db_t;
    Tb_table* tb_t;

public:

    All_db ()
    {
        db_name = "information_schema";
        db_t = new DB_table;
        tb_t = new Tb_table;
        tb_vec = {db_t, tb_t};
    }

    void insert (std::vector<std::string> l1, 
                 std::vector<uint32_t> l2, uint8_t tb_num) override
    {
        tb_vec[tb_num]->insert(l1, l2);
        // smth with tables_db
    }

    void write () override { db_first_write(*this); }
    void read (std::string name) override 
    { db_read(*this, name); }


    ~All_db ()
    {
        delete db_t;
        delete tb_t;
    }

};

// Faculties
class Virtual_faculty : public Database
{
public:

    /*
    Virtual_faculty ()
    {
        Fac_table fac_t;
        Dep_table dep_t;
        Dis_table dis_dep_t;
        tb_vec = {fac_t, dep_t, dis_dep_t};
    }
    */
};

class Common_faculty : public Virtual_faculty // schema 1 type
{

};

class Special_faculty : public Virtual_faculty // schema 2 type
{
public:
    //Borg_table borg_t;
    //Dis_table dis_borg_t;
    //std::vector<Table> tb_vec;
};

#endif
