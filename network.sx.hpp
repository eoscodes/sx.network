#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>

#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("network.sx")]] networkSx : public contract {
public:
    using contract::contract;
    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name from, const name to, const asset quantity, const string memo );

private:
    void self_transfer( const name to, const name contract, const asset quantity, const string memo );
    symbol_code parse_memo_symcode( const string memo );
    name get_contract( const name contract, const symbol_code symcode );
};
