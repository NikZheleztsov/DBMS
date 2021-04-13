#ifndef TUPLE_H
#define TUPLE_H
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <utility>
#include <iomanip>
#include "tuple.h"

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
    virtual std::pair<std::vector<std::string>,
            std::vector<uint32_t>> get_data() const = 0;

    friend class Table;
    friend class DB_table;
    friend void db_full_write (Database& db);
    friend void show_databases ();
    friend void check_for_init();
    friend void parsing_in (std::vector<std::string> all_words);

    virtual ~tuple () {};
};

//schemata
class dbases : public tuple // type 0
{
    uint8_t schema_type;

public:

    dbases(std::fstream& in);

    dbases (std::string n, uint8_t type) : tuple(33, n), schema_type(type) {}
    dbases (dbases& db) : tuple(33, db.name), schema_type(db.schema_type) {};

    void write(std::fstream& out) override;
    std::vector<std::string> print () const override;
    std::pair<std::vector<std::string>,
        std::vector<uint32_t>> get_data() const override;
};

// faculty
class fac : public tuple // 1 type
{
    std::string name_nuc;
    uint32_t num_dep;
    bool is_spec;

public:

    fac (std::fstream& in);
    fac (std::string n, std::string nuc, uint32_t dep, bool spec) : 
        tuple(69, n), name_nuc(nuc), num_dep(dep), is_spec(spec) {}

    std::vector<std::string> print () const override;
    void write(std::fstream& out) override;
    std::pair<std::vector<std::string>,
        std::vector<uint32_t>> get_data() const override;

};

// department
class dep : public tuple// 2 type
{
public:

    dep (std::fstream& in);
    dep (std::string n, uint32_t id) : tuple(36, n, id) {}
    void write(std::fstream& out) override;
    std::pair<std::vector<std::string>,
        std::vector<uint32_t>> get_data() const override;
    std::vector<std::string> print () const override;
};

// base organization
class borg : public tuple// 3 type
{
public:

    borg (std::fstream& in);
    borg (std::string n, uint32_t id) : tuple(36, n, id) {}
    void write(std::fstream& out) override;
    std::vector<std::string> print () const override;
    std::pair<std::vector<std::string>,
        std::vector<uint32_t>> get_data() const override;
};

// discipline
class dis : public tuple // 4 type
{
    uint32_t dis_teach_num;

public:
    dis (std::fstream& in);
    dis (std::string n, uint32_t teach, uint32_t id) : tuple(40, n, id), 
            dis_teach_num(teach) {}

    std::vector<std::string> print () const override;
    void write(std::fstream& out) override;
    std::pair<std::vector<std::string>,
        std::vector<uint32_t>> get_data() const override;

};

#endif
