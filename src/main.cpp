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
    std::cout << "Welcome to DBMS (pre-alpha). \n\
Type 'help' for more information\n\
Use 'q' or 'quit' in order to exit\n\n";
    std::string answ = "";
    while (answ != "quit" && answ != "q")
    { 
        std::cout << "DBMS [(" << curr_db << ")]> ";
        answ.clear();

        do {
            std::string temp;
            std::getline(std::cin, temp);
            answ += temp;
        } while (answ[answ.size() - 1] != ';' && !(answ == "q" 
                || answ == "quit" || answ == "help"));

        if (answ == "help")
        {
            help();
            continue;
        }

        if (answ != "quit" && answ != "q")
        {
            parsing(answ);

        } else {

            std::cout << "Saving databases...\n";

            if (current_db != nullptr)
                current_db->write();

            if (db != nullptr)
                db->write();
        }
    }

    return 0;
}

