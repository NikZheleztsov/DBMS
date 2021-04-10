#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

class Database;

/*
template <typename T, typename Y>
bool universal_comp(T& data, char& sign, Y& cond)
{
    if (static_cast<T&>(cond))
    {
        switch(sign)
        {
            case '>':
                return (data > cond);
            
            case '<':
                return (data < cond);

            case '=':
                return (data == cond);
        }
    } else 
        throw std::invalid_argument("Comparison error");
}
*/

class tuple 
{
protected:
    uint32_t size = 0;

public:
    std::string name = ""; // 32 byte

    tuple () {}
    tuple(uint32_t s, std::string n) : size(s), name(n) {}
    virtual void write(std::fstream& out) = 0;

    virtual std::vector<std::string> print (int8_t num_of_col, 
            char sign, int64_t cond) const = 0;

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

    std::vector<std::string> print (int8_t num_of_col, 
            char sign, int64_t cond) const override
    {
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

        size = 67;
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
        tuple(67, n), name_nuc(nuc), num_dep(dep), is_spec(spec) {}

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

    std::vector<std::string> print (int8_t num_of_col, 
            char sign, int64_t cond) const override
    {
    }
};

// department
class dep : public tuple// 2 type
{
    //std::string dep_name;
    uint32_t fac_id;

public:

    dep (std::fstream& in) {

        size = 36;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t fac = 0;
        in.read(reinterpret_cast<char*>(&fac), 4);

        this->name = name;
        fac_id = fac;
        delete [] name;
    }

    dep (std::string n, uint32_t id) : tuple(36, n), fac_id(id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&fac_id), 4);
    }

    std::vector<std::string> print (int8_t num_of_col, char sign, int64_t cond) const override
    {
    }
};

// base organization
class borg : public tuple// 3 type
{
    //std::string borg_name;
    // (optional foreign key for preformance increase)
    uint32_t fac_id;

public:

    borg (std::fstream& in) {

        size = 36;
        char* name = new char [32];
        in.read(name, 32);
        uint32_t fac = 0;
        in.read(reinterpret_cast<char*>(&fac), 4);

        this->name = name;
        fac_id = fac;
        delete [] name;
    }

    borg (std::string n, uint32_t id) : tuple(36, n), fac_id(id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&fac_id), 4);
    }

    std::vector<std::string> print (int8_t num_of_col, 
            char sign, int64_t cond) const override
    {
    }
};

// discipline
class dis : public tuple // 4 type
{
    // std::string dis_name;
    uint32_t dis_teach_num;
    uint32_t foreign_id;

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
        foreign_id = id;
        delete [] name;
    }

    dis (std::string n, uint32_t teach, uint32_t id) : tuple(40, n), 
            dis_teach_num(teach), foreign_id(id) {}

    void write(std::fstream& out) override
    {
        out.write(name.c_str(), name.size());
        char* bl = new char [32 - name.size()] ();
        out.write(bl, 32 - name.size());
        delete [] bl;

        out.write(reinterpret_cast<char*>(&dis_teach_num), 4);
        out.write(reinterpret_cast<char*>(&foreign_id), 1);
    }

    std::vector<std::string> print (int8_t num_of_col, 
            char sign, int64_t cond) const override
    {
    }
};

#endif
