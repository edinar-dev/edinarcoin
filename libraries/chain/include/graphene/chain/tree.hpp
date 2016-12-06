#pragma once

#include <list>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/protocol/asset.hpp>
#include "tree.hh"

#define PRECISION 1000

namespace graphene { namespace chain {
    
class referral_info {
    public:
    referral_info(account_id_type account_id, 
                  uint64_t quantity,
                  string rank,
                  vector<child_balance> cb = vector<child_balance>()): 
            to_account_id(account_id), 
            quantity(quantity),
            history(cb),
            rank(rank)   { }
    account_id_type to_account_id;
    vector<child_balance> history;
    uint64_t quantity; 
    string rank;
};

class referral_tree {
    public:
    tree<leaf_info> tree_data;
    tree<leaf_info>::iterator root;
    std::map<account_id_type, tree<leaf_info>::iterator> referral_map;
    const account_index& accounts_idx;
    const account_balance_index& balances_idx;
    const asset_id_type asset_id;
    account_id_type root_account;
    tree<leaf_info> form();
    std::list<referral_info> scan();
    referral_tree(const account_index& accs, const account_balance_index& bals, 
                  asset_id_type asst, account_id_type root_account = account_id_type())
                  : accounts_idx(accs), balances_idx(bals), asset_id(asst), root_account(root_account)
    {
        const uint64_t zero_account_balance = get_balance(root_account).amount.value;
        tree<leaf_info>::iterator top = tree_data.begin();
        root = tree_data.insert(top, leaf_info(root_account, zero_account_balance));
        referral_map.insert(std::pair<account_id_type, tree<leaf_info>::iterator>(root_account, root));
    }
    asset get_balance(account_id_type owner);
    void set_bonus_percents();
};

}}