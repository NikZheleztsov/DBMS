#ifndef FS_WORK_H
#define FS_WORK_H
#include "databases.h"

void db_full_write (Database& db);
Database* db_read (Database* db, std::string name);

#endif
