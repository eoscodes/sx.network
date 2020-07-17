#include <eosio.token/eosio.token.hpp>
#include <sx.swap/swap.sx.hpp>
#include <sx.registry/registry.sx.hpp>

#include "network.sx.hpp"

[[eosio::on_notify("*::transfer")]]
void networkSx::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore EOSIO system accounts
    const set<name> ignore = set<name>{ "eosio.stake"_n, "eosio.ram"_n, "eosio.rex"_n };
    if ( ignore.find( from ) != ignore.end() ) return;

    // ignore incoming registry.sx accounts
    registrySx::swap_table _registry( "registry.sx"_n, "registry.sx"_n.value );
    if ( _registry.find( from.value ) != _registry.end() ) return;

    // ignore transfers
    if ( to != get_self() || memo == get_self().to_string() ) return;

    // add docs
    docs _docs( get_self(), get_self().value );
    if ( !_docs.exists() ) _docs.get_or_create( get_self() );

    // user input validation
    const name contract = get_first_receiver();
    const symbol_code symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );

    // calculate best rates
    name best_contract;
    extended_asset best_rate;
    for ( const auto registry : _registry ) {
        if ( !registry.tokens.count( symcode ) ) continue;
        if ( !registry.tokens.count( out_symcode ) ) continue;
        const asset rate = swapSx::get_rate( registry.contract, quantity, out_symcode );
        const name token_contract = swapSx::get_contract( registry.contract, out_symcode );
        const asset balance = eosio::token::get_balance( token_contract, registry.contract, out_symcode );

        // insufficient balance
        if ( balance < rate ) continue;

        // update with best rate
        if ( rate.amount > best_rate.quantity.amount ) {
            best_contract = registry.contract;
            best_rate = extended_asset{ rate, token_contract };
        }
    }
    check( best_contract.value, "network cannot match from `" + symcode.to_string() + "` to `" + out_symcode.to_string() + "`");

    // convert & send funds back to user
    self_transfer( best_contract, extended_asset{ quantity, contract }, out_symcode.to_string() );
    self_transfer( from, best_rate, "convert" );
}

void networkSx::self_transfer( const name to, const extended_asset ext_quantity, const string memo )
{
    eosio::token::transfer_action transfer( ext_quantity.contract, { get_self(), "active"_n });
    transfer.send( get_self(), to, ext_quantity.quantity, memo );
}

symbol_code networkSx::parse_memo_symcode( const string memo )
{
    const string error = "`memo` must be symbol code (ex: \"USDT\")";
    check( memo.length() > 0 && memo.length() <= 7 , error );

    // must be all uppercase alpha characters
    for (char const c: memo ) {
        check( isalpha(c) && isupper(c), error );
    }
    const symbol_code symcode = symbol_code{ memo };
    check( symcode.is_valid(), error );
    return symcode;
}