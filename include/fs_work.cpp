#include "databases.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <bitset>
#include <algorithm>

namespace fs = std::filesystem;
extern fs::path root;
extern uint32_t block_size;
extern std::string config_name,
            db_dir;

// from 1 !!!
// in order to write to the end of db_meta_block and distinguish from \0 ... \0
std::bitset<256> bit_blocks;

class data_block 
{
    int pointer = 0;

public:

    char* data;
    uint8_t num_of_block;

    data_block()
    {
        data = new char [block_size];
    }

    // 1 if out of size
    bool insert (std::pair<uint32_t, tuple*> tuple, uint32_t table_id)
    {
        if (pointer == 0)
        {
           data[pointer] = table_id; 
           pointer += 4;
        }

        data[pointer] = tuple.first;
        if ((block_size - pointer) > tuple.second->size)
        {
            pointer = tuple.second->write(&data[pointer + 4]);
            return 0;
        }

        return 1;
    }

    // double free if not commented
    /*
    ~data_block()
    {
        delete [] data;
    }
    */
};

void db_first_write (Database& db)
{
    if (fs::exists(root/db_dir))
    {
        std::fstream file (root/db_dir/db.db_name, std::ios::binary | std::ios::out);
        if (file.is_open())
        {
            // first block is always 256 bytes
            // 4 bytes - num_of_block
            // db_name <= 248 !!!
            // 4 bytes for bl_size
            
            uint8_t bl_num = 8;
            file.write(reinterpret_cast<char*>(&bl_num), 1);
            file.write(reinterpret_cast<char*>(&block_size), 4); 

            file.write(db.db_name.c_str(), db.db_name.size());

            uint8_t pad_size = 251 - db.db_name.size();
            char * padding = new char [pad_size] ();
            file.write(padding, pad_size);
            delete [] padding;

            // bitset
            char * bitset = new char [32] ();
            file.write(bitset, 32);
            delete [] bitset;

            // table meta block
            // size = block_size
            // max num of tables = 8 [0 - 7]
            // tb_name <= 64
            // col_name <= 32
            
            std::vector <std::vector<data_block>> tb_data_vec;

            for (auto x : db.tb_vec)
            {
                file.write(x->table_name.c_str(), x->table_name.size());

                pad_size = 64 - x->table_name.size();
                char * padding = new char [pad_size] ();
                file.write(padding, pad_size);
                delete [] padding;

                int num_of_col_names = 0;
                for (auto y : x->col_names)
                {
                    file.write(y.c_str(), y.size());

                    pad_size = 32 - y.size();
                    char * padding = new char [pad_size] ();
                    file.write(padding, pad_size);
                    delete [] padding;

                    num_of_col_names ++;
                }

                std::vector<data_block> data_block_vec;
                auto y = x->tuple_map.begin();
                bool cont_fl = 1;
                while (cont_fl)
                {
                    data_block bl;
                    
                    cont_fl = 0;
                    for (; y != x->tuple_map.end(); y++)
                    {
                        if (bl.insert(*y, x->table_type))
                        {
                            cont_fl = 1;
                            break;
                        }
                    }

                    data_block_vec.push_back(bl);
                }

                tb_data_vec.push_back(data_block_vec);

                int size_of_pointers_bl = block_size - 64 - 32 * num_of_col_names;
                char * pointers = new char [size_of_pointers_bl] ();
                int n = 1;
                for (auto z : data_block_vec)
                {
                    uint8_t j = 0;
                    for(j = 0; bit_blocks[j]; j++); //first ~1 bit
                    z.num_of_block = j + 1;
                    bit_blocks.set(j);
                    pointers[size_of_pointers_bl - n] = j + 1;
                    n++;
                }

                file.write(pointers, size_of_pointers_bl);

                delete [] pointers;
            }

            for (int i = 0; i < 8 - db.tb_vec.size(); i++)
            {
                char* empty_meta_table_block = new char [block_size] ();
                file.write(empty_meta_table_block, block_size);
            }

            // ascending order of data blocks
            for (auto z : tb_data_vec)
            {
                std::sort(z.begin(), z.end(), [] (data_block d1, data_block d2) {
                        return (d1.num_of_block < d2.num_of_block); });
            }


            std::sort(tb_data_vec.begin(), tb_data_vec.end(), [] (std::vector<data_block> d1,
                                                                  std::vector<data_block> d2) {
                        return ((d1.size() != 0 && d2.size() != 0) ? 
                               (d1[0].num_of_block < d2[0].num_of_block) :
                                0);  });

            // writing datablocks
            for (auto a : tb_data_vec)
            {
                for (auto b : a)
                {
                    file.seekp(256 + 32 + block_size * (8 + b.num_of_block - 1));
                    file.write(b.data, block_size);
                }
            }

            // bitset of blocks
            file.seekp(256);
            file.write(bit_blocks.to_string().c_str(), 32);

            file.close();
        } else {
            std::cout << "Unable to create a database file\n";
        }

    } else {
        std::cout << "Unable to find directory with databases\nExiting ...\n";
    }
}

void db_read (Database& db, std::string name) 
{
    if (fs::exists(root/db_dir/name))
    {
        std::fstream file (root/db_dir/name, std::ios::binary | std::ios::in);
        if (file.is_open())
        {
            
        } else {
            std::cout << "Unable to open the database\nExiting ...\n";
        }

    } else {
        std::cout << "Unable to find directory with databases\nExiting ...\n";
    }
}
