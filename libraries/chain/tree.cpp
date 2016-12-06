#include <graphene/chain/tree.hpp>

namespace graphene { namespace chain {

asset referral_tree::get_balance(account_id_type owner) {

   auto& idx = balances_idx.indices().get<by_account_asset>();
   auto itr = idx.find(boost::make_tuple(owner, asset_id));
   if( itr == idx.end() )
      return asset(0, asset_id);
   return itr->get_balance();
}

tree<leaf_info> referral_tree::form() 
{
   const auto& idx = accounts_idx.indices().get<by_id>();
   
   for (auto account = ++idx.begin(); account != idx.end(); account++) {
      tree<leaf_info>::iterator referrer;
      auto referrer_from_map = referral_map.find(account->referrer);
      if (referrer_from_map == referral_map.end()) {
         if (root_account == account_id_type())
            referrer = root;
         else continue;
      } else {
         referrer = referrer_from_map->second;
      }
      
      const uint64_t account_balance = get_balance(account->get_id()).amount.value;
      auto account_pos = tree_data.append_child(referrer, leaf_info(account->get_id(), account_balance));
      referral_map.insert(std::pair<account_id_type, tree<leaf_info>::iterator>(account->get_id(), account_pos));
      
      int level = 1;
      for (auto& current_node = account_pos; ; level++) {
         const auto& parent_node = tree_data.parent(current_node);
         if (parent_node == nullptr) break;
      
         uint64_t current_balance = get_balance(current_node->account_id).amount.value;
         parent_node->add_child_balance(account->id, account_balance, level);
         current_node = parent_node;
      }
   }
   set_bonus_percents();
   return tree_data;
}

std::list<referral_info> referral_tree::scan() 
{
   std::list<referral_info> operations_storage;
   for (auto& leaf: tree_data) {
      if (leaf.balance < 200 * PRECISION) continue;
      if (leaf.level_1_partners < 5) continue;          
      uint64_t bonus = leaf.get_bonus_value();
      if (bonus < 1) continue;  
      operations_storage.push_back(referral_info(leaf.account_id, bonus, leaf.rank, leaf.get_child_balances() ));
   }
   return operations_storage;
}

std::vector<child_balance> leaf_info::get_child_balances() {
    std::vector<child_balance> result;
    for (auto cb: child_balances) {
        cb.balance = cb.balance * 0.0065 * bonus_percent;
        if (cb.balance > 0 && (bonus_percent < 0.25 || cb.level == 1) )
            result.push_back(cb);
    }
    return result;
}

void referral_tree::set_bonus_percents() 
{
   for (auto& leaf: tree_data) {
      if (leaf.balance < 200 * PRECISION) continue;
      if (leaf.level_1_partners < 5) continue;  
      if (leaf.level_2_partners >= 25) {
         if (leaf.balance < 500 * PRECISION) continue;
         if (leaf.all_partners < 125) {
            leaf.rank = "B";
            leaf.bonus_percent = 0.2;
         } else if (leaf.all_partners < 625) {
            if (leaf.balance >= 1000 * PRECISION) {
               leaf.rank = "C";
               leaf.bonus_percent = 0.15;
            } 
         } else if (leaf.all_partners < 3125) {
            if (leaf.balance >= 2000 * PRECISION) {
               leaf.rank = "D";
               leaf.bonus_percent = 0.10;
            }
         } else if (leaf.all_partners < 15625) {
            if (leaf.balance >= 3000 * PRECISION) {
               leaf.rank = "E";
               leaf.bonus_percent = 0.05;
            }
         } else if (leaf.all_partners < 78125) {
            if (leaf.balance >= 4000 * PRECISION) {
               leaf.rank = "F";
               leaf.bonus_percent = 0.025;
            }
         } else {
            if (leaf.balance >= 5000 * PRECISION) {
               leaf.rank = "G";
               leaf.bonus_percent = 0.025;
            }
         }
      } else {
         leaf.rank = "A";
         leaf.bonus_percent = 0.25;
      }        
   }
}

void leaf_info::add_child_balance(chain::account_id_type account_id, uint64_t balance, uint32_t level) 
{
   all_sum += balance;
   child_balances.push_back(child_balance(account_id, balance, level));
   if (level == 1)
      level_1_sum += balance;
   if (balance < 100 * PRECISION) return;
   all_partners++;
   if (level == 1)
      level_1_partners++;
   if (level == 2) level_2_partners++;
}
int64_t leaf_info::get_bonus_value() 
{
   if (level_2_partners >= 25) {
      return all_sum * 0.0065 * bonus_percent;
   } else {
      return level_1_sum * 0.0065 * bonus_percent;
   }
}

}}
