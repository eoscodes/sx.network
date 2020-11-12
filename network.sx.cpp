#include <eosio.token/eosio.token.hpp>
#include <sx.swap/swap.sx.hpp>
#include <sx.stats/stats.sx.hpp>

#include "network.sx.hpp"

[[eosio::on_notify("*::transfer")]]
void networkSx::on_transfer( const name from, const name to, const asset quantity, const string memo )
{
    // authenticate incoming `from` account
    require_auth( from );

    // ignore EOSIO system accounts & DEX
    const set<name> ignore = set<name>{ "eosio.ram"_n };
    if ( ignore.find( from ) != ignore.end() ) return;

    // SX swap contracts
    const set<name> swap_contracts = {"stable.sx"_n, "swap.sx"_n};

    // ignore incoming registry.sx accounts
    if ( swap_contracts.find( from ) != swap_contracts.end() ) return;

    // ignore transfers
    if ( to != get_self() || memo == get_self().to_string() ) return;

    // user input validation
    const name contract = get_first_receiver();
    const symbol_code in_symcode = quantity.symbol.code();
    const symbol_code out_symcode = parse_memo_symcode( memo );

    // find best rates
    name best_contract;
    asset best_rate;
    asset fee;
    name token_contract;

    for ( const name swap_contract : swap_contracts ) {
        sx::swap::settings _settings( swap_contract, swap_contract.value );
        sx::swap::tokens _tokens( swap_contract, swap_contract.value );

        if ( _tokens.find( in_symcode.raw() ) == _tokens.end() ) continue;
        if ( _tokens.find( out_symcode.raw() ) == _tokens.end() ) continue;

        token_contract = _tokens.get( out_symcode.raw() ).contract;
        const asset rate = sx::swap::get_amount_out( swap_contract, quantity, out_symcode );
        const asset balance = eosio::token::get_balance( token_contract, swap_contract, out_symcode );

        // insufficient balance
        if ( balance < rate ) continue;

        // update with best rate
        if ( rate.amount > best_rate.amount ) {
            best_contract = swap_contract;
            best_rate = rate;
            fee = quantity * _settings.get().fee / 10000;
        }
    }
    check( best_contract.value, "network cannot match from `" + in_symcode.to_string() + "` to `" + out_symcode.to_string() + "`");

    // convert & send funds back to user
    self_transfer( best_contract, contract, quantity, out_symcode.to_string() );
    self_transfer( from, token_contract, best_rate, "convert" );

    // push log
    sx::stats::swaplog_action swaplog( "stats.sx"_n, { get_self(), "active"_n });
    swaplog.send( best_contract, from, quantity, best_rate, fee );
}

void networkSx::self_transfer( const name to, const name contract, const asset quantity, const string memo )
{
    eosio::token::transfer_action transfer( contract, { get_self(), "active"_n });
    transfer.send( get_self(), to, quantity, memo );
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