#pragma once

#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("network.sx")]] networkSx : public contract {
public:
    using contract::contract;

    /**
     * ## TABLE `docs`
     *
     * - `{string} url` - Documentation url
     *
     * ### example
     *
     * ```json
     * {
     *   "url": "https://github.com/stableex/sx.network"
     * }
     * ```
     */
    struct [[eosio::table("docs")]] docs_row {
        string      url = "https://github.com/stableex/sx.network";
    };
    typedef eosio::singleton< "docs"_n, docs_row > docs;

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::transfer")]]
    void on_transfer( const name from, const name to, const asset quantity, const string memo );

private:
    void self_transfer( const name to, const extended_asset ext_quantity, const string memo );
    symbol_code parse_memo_symcode( const string memo );
};
