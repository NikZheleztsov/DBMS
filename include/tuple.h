#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
// #include "databases.h"

class Database;

class tuple 
{
protected:
    uint32_t size = 0;
    int32_t for_key = -1; // foreign_key (if == -1 then doesn't used)

public:
    std::string name = ""; // 32 byte

    tuple () {}
    tuple(uint32_t s, std::string n, int32_t key = -1) : 
        size(s), name(n), for_key(key) {}
    virtual void write(std::fstream& out) = 0;
    virtual std::vector<std::string> print () const = 0;

    friend class Table;
    friend class DB_table;
    friend void db_full_write (Database& db);
    friend void show_databases ();
    friend void check_for_init();

    virtual ~tuple () {};
};

//schemata
class dbases : public tuple // type 0
{
    uint8_t schema_type;

public:

    dbases(std::fstream& in) {

        size = 33;
        char* name = new char [32];
        in.read(name, 32);
        uint8_t type = 0;
        in.read(reinterpret_cast<char*>(&type), 1);

        this->name = name;
        schema_type = type;
        delete [] name;
    }

    dbases (std::string n, uint8_t type) : tuple(33, n), schema_type(type) {}
    dbases (dbases& db) : tuple(33, db.name), schema_type(db.schema_type) {};

    // name <= 32
    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;
        out.write(reinterpret_cast<char*>(&schema_type), 1);
    }

    std::vector<std::string> print () const override
    {
        std::vector<std::string> out = 
            {name, std::to_string(schema_type)};

        return out;
    }
};

/*
class tb : public tuple
{
    //std::string table_name;
    uint32_t rows_num;
    uint32_t db_id;

public: 

    tb (std::fstream& in) {

        size = 40;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t rows = 0, id = 0;
        in.read(reinterpret_cast<char*>(&rows), 4);
        in.read(reinterpret_cast<char*>(&id), 4);

        this->name = name;
        rows_num = rows;
        db_id = id;
        delete [] name;
    }

    tb (std::string n, uint32_t rows, uint32_t id) : tuple(40, n), rows_num(rows), db_id(id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;
        out.write(reinterpret_cast<char*>(&rows_num), 4);
        out.write(reinterpret_cast<char*>(&db_id), 4);
    }

    void print (uint8_t width, std::vector <uint8_t> num, int8_t num_of_col, 
            char sign, int64_t cond) const override
    {
        if (num_of_col == -1 || sign == ' ' || cond == -1)
            std::cout << std::setw(width) << name <<
                std::setw(width) << rows_num << 
                std::setw(width) << db_id << std::endl;
    }
};
*/

// faculty
class fac : public tuple // 1 type
{
    //std::string name_fac;
    std::string name_nuc;
    uint32_t num_dep;
    bool is_spec;

public:

    fac (std::fstream& in) {

        size = 69;
        char* name = new char [32];
        in.read(name, 32);
        this->name = name;

        in.read(name, 32);
        name_nuc = name;
        delete[] name;

        uint32_t num_dep = 0;
        bool spec = false;
        in.read(reinterpret_cast<char*>(&num_dep), 4);
        in.read(reinterpret_cast<char*>(&spec), 1);
        this->num_dep = num_dep;
        is_spec = spec;
    }

    fac (std::string n, std::string nuc, uint32_t dep, bool spec) : 
        tuple(69, n), name_nuc(nuc), num_dep(dep), is_spec(spec) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(name_nuc.c_str(), name.size());
        bl = new char [32 - name_nuc.size()] ();
        out.write(bl, 32 - name_nuc.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&num_dep), 4);
        out.write(reinterpret_cast<char*>(&is_spec), 1);
    }

    std::vector<std::string> print () const override
    {
        std::vector<std::string> out = 
        {name, name_nuc, std::to_string(num_dep),
            std::to_string(is_spec)};

        return out;
    }
};

// department
class dep : public tuple// 2 type
{
    //std::string dep_name;
    // uint32_t fac_id;

public:

    dep (std::fstream& in) {

        size = 36;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t fac = 0;
        in.read(reinterpret_cast<char*>(&fac), 4);

        this->name = name;
        for_key = fac;
        delete [] name;
    }

    dep (std::string n, uint32_t id) : tuple(36, n, id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&for_key), 4);
    }

    std::vector<std::string> print () const override
    {
        std::vector<std::string> out = 
        {name, std::to_string(for_key)};

        return out;
    }
};

// base organization
class borg : public tuple// 3 type
{
    //std::string borg_name;
    // uint32_t fac_id;

public:

    borg (std::fstream& in) {

        size = 36;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t fac = 0;
        in.read(reinterpret_cast<char*>(&fac), 4);

        this->name = name;
        for_key = fac;
        delete [] name;
    }

    borg (std::string n, uint32_t id) : tuple(36, n, id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&for_key), 4);
    }

    std::vector<std::string> print () const override
    {
        std::vector<std::string> out = 
        {name, std::to_string(for_key)};

        return out;
    }
};

// discipline
class dis : public tuple // 4 type
{
    // std::string dis_name;
    uint32_t dis_teach_num;
    // uint32_t foreign_id;

public:
    dis (std::fstream& in) {

        size = 40;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t teach = 0, id = 0;
        in.read(reinterpret_cast<char*>(&teach), 4);
        in.read(reinterpret_cast<char*>(&id), 4);

        this->name = name;
        dis_teach_num = teach;
        for_key = id;
        delete [] name;
    }

    dis (std::string n, uint32_t teach, uint32_t id) : tuple(40, n, id), 
            dis_teach_num(teach) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&dis_teach_num), 4);
        out.write(reinterpret_cast<char*>(&for_key), 1);
    }

    std::vector<std::string> print () const override
    {
        std::vector<std::string> out = 
        {name, std::to_string(dis_teach_num),
            std::to_string(for_key)};

        return out;
    }
};

#endif
