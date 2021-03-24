// discipline
struct dis
{
    uint8_t dis_id;
    std::string dis_name;
    uint8_t dis_teach_num;
}

// department
struct dep
{
    uint8_t dep_id;
    std::string dep_name;
    std::vector<dis> dep_ls_dis;
}

struct base_org
{
    uint8_t borg_id;
    std::string borg_name;
    std::vector<dis> borg_ls_dis;
}

// interface
class virt_fac
{

protected:
    uint8_t fac_id;
    std::string name_fac;
    std::string name_nuc;
    uint8_t num_dep;
    std::vector<dep> ls_dep;

public:

}

class base_fac : public virt_fac
{
}

class spec_fac : public virt_fac
{
    std::vector<base_org> ls_borg;
}
