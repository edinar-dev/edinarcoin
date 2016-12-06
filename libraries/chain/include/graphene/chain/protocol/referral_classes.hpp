#pragma once 

#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/memo.hpp>

namespace graphene { namespace chain { 

class child_balance {
public:
    account_id_type account_id;
    uint64_t balance;
    uint32_t level;
    child_balance(): account_id(account_id_type()),balance(0), level(0) {};
    child_balance(account_id_type acc_id, uint64_t bal, uint32_t lvl): account_id(acc_id), balance(bal), level(lvl) {};
};

class leaf_info {
public:
    typedef account_id_type acc_id_type;
    account_id_type account_id = account_id_type();
    uint64_t balance = 0;
    uint32_t level_1_partners = 0;
    uint64_t level_1_sum = 0;
    uint32_t level_2_partners = 0;
    uint32_t all_partners = 0;
    uint64_t all_sum = 0;
    double bonus_percent = 0;
    string rank;
    std::vector<child_balance> child_balances;
    leaf_info(account_id_type acc_id, uint64_t bal): account_id(acc_id), balance(bal) {};
    leaf_info( account_id_type acc_id, uint64_t bal, 
               uint32_t level_1_partners, uint64_t level_1_sum, 
               uint32_t level_2_partners, 
               uint32_t all_partners, uint64_t all_sum, 
               double bonus_percent) : 
        account_id(acc_id), balance(bal),
        level_1_partners(level_1_partners), level_1_sum(level_1_sum), 
        level_2_partners(level_2_partners), 
        all_partners(all_partners), all_sum(all_sum), 
        bonus_percent(bonus_percent) {};
    leaf_info() {};
    int64_t get_bonus_value();
    void add_child_balance(account_id_type account_id, uint64_t balance, uint32_t level);
    std::vector<child_balance> get_child_balances();
    bool operator ==(leaf_info& other)
    {
        return this->account_id == other.account_id &&
            this->all_partners == other.all_partners &&
            this->all_sum == other.all_sum &&
            this->balance == other.balance &&
            this->bonus_percent == other.bonus_percent &&
            this->level_1_partners == other.level_1_partners &&
            this->level_1_sum == other.level_1_sum &&
            this->level_2_partners == other.level_2_partners;
    }
};
} }

FC_REFLECT( graphene::chain::child_balance,
            (account_id)(balance)(level) )

FC_REFLECT( graphene::chain::leaf_info,
            (account_id)(rank)(balance)(level_1_partners)(level_1_sum)(level_2_partners)(all_partners)(all_sum)(bonus_percent) )