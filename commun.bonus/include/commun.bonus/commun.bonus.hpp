#pragma once

#include <commun/upsert.hpp>
#include <commun/config.hpp>
#include <commun/dispatchers.hpp>

#include <eosio/time.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/binary_extension.hpp>
#include <string>

namespace commun {
using namespace eosio;

/**
  \brief This table holds info about users' funds and release time

  \ingroup bonus_tables
*/
struct account_info {
    uint64_t id;
    asset   quantity;            //!< bonus quantity
    time_point_sec release;      //!< release date for bonus

    uint64_t primary_key() const {return id;}
    symbol_code symbol() const {return quantity.symbol.code();}
};

using account_release_idx [[using eosio: non_unique, order("quantity._sym"), order("release")]] = 
        indexed_by<"byrelease"_n, composite_key<account_info,
            const_mem_fun<account_info, symbol_code, &account_info::symbol>,
            member<account_info, time_point_sec, &account_info::release>>>;
using account_tbl [[using eosio: order("id","asc"), contract("commun.bonus")]] = 
        eosio::multi_index<"accounts"_n, account_info, account_release_idx>;

/**
 * \brief This class implements the \a c.bonus contract functionality.
 * \ingroup bonus_class
 */
class
/// @cond
[[eosio::contract("commun.bonus")]]
/// @endcond
bonus: public contract {
    
public:
    bonus(name self, name code, datastream<const char*> ds)
        : contract(self, code, ds)
    {
    }

    /**
     * \brief The \ref claim action is used by \a name to receive asset hold by the contract.
     *
     * \param account user account
     *
     * \param symbol_code a token symbol
     *
     * \signreq
            — the \a name contract account .
     */
    [[eosio::action]] void claim(name account, symbol_code token_code);

    ON_TRANSFER(CYBER_TOKEN) void on_tokens_transfer(name from, name to, asset quantity, std::string memo);

};

} // commun
