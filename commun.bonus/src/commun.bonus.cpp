#include "commun.bonus/commun.bonus.hpp"
#include <commun/config.hpp>
#include <eosio/transaction.hpp>
#include <eosio/event.hpp>
#include <eosio/permission.hpp>
#include <eosio/crypto.hpp>
#include <cyber.token/cyber.token.hpp>

namespace commun {

using namespace eosio;

void bonus::claim(name account, symbol_code token_code) {
    require_auth(account);
    
    account_tbl table(_self, account.value);
    auto idx = table.get_index<"byrelease"_n>();
    auto itr = idx.lower_bound(std::make_tuple(token_code, time_point_sec()));
    eosio::check(itr != idx.end() && itr->symbol() == token_code, "Nothing to claim");
    eosio::check(itr->release <= current_time_point(), "Too early to claim");

    INLINE_ACTION_SENDER(token, transfer) (config::token_name, {_self, config::active_name},
            {_self, account, itr->quantity, ""});
}

void bonus::on_tokens_transfer(name from, name to, asset quantity, std::string memo) {
    auto token_code = quantity.symbol.code();
    if (_self != to) {
        // transfer funds from contract
        account_tbl table(_self, to.value);
        auto idx = table.get_index<"byrelease"_n>();
        auto itr = idx.lower_bound(std::make_tuple(token_code, time_point_sec()));
        eosio::check(itr != idx.end() && itr->symbol() == token_code, "Nothing to transfer");
        eosio::check(itr->release <= current_time_point(), "Too early to transfer");
        eosio::check(quantity <= itr->quantity, "Too many funds requested");

        if (quantity < itr->quantity) idx.modify(itr, eosio::same_payer, [&](auto& w) { w.quantity -= quantity; });
        else idx.erase(itr);

        return;
    }

    char account_str[16];
    uint32_t release_raw;
    int r = sscanf(memo.c_str(), "to:%15s release:%u", account_str, &release_raw);
    eosio::check(r == 2, "Invalid memo string. Available format: 'to: <account> release: <unix-timestamp>'");

    name account = name(account_str);
    eosio::check(is_account(account), "Missing recipient account");
    eosio::check(account != _self, "Can't hold funds for self");

    account_tbl table(_self, account.value);
    table.emplace(_self, [&](auto& v) {
            v.id = table.available_primary_key();
            v.quantity = quantity;
            v.release = time_point_sec(release_raw);
        });
}


} // commun
