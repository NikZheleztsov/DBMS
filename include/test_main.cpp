#include "databases.h"
#include "misc.h"

/* Db types:
   0 - meta db
   1 - common fac
   2 - hybrid fac
*/

Database* db = nullptr;
Database* current_db = nullptr;
std::string curr_db = "none";

int main ()
{
    check_for_init();
    current_db = new Database(1, "common faculty");
    current_db->insert(1,
}
