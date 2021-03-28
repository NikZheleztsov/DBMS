#include "databases.h"
#include "misc.h"

/* Db types:
   0 - meta db
   1 - common fac
   2 - hybrid fac
*/

int main ()
{
    check_for_init();
    // if ../.databases/ is empty launch init_DBMS + config
    All_db db;
    // inserting to database_t
    db.insert({"schemata"}, {0}, 0);
    db.write();

    All_db new_db;
    new_db.read("information_schema");
    return 0;
}

