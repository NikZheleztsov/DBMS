#ifndef FACULTIES_H
#define FACULTIES_H
#include "table.h"
#include "tuple.h"

class Virtual_faculty
{
    Table<Fac> fac_table;
    Table<Dep> dep_table;
    Table<Dis> dis_table;
};

#endif
