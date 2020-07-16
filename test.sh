#!/bin/bash

# compile
eosio-cpp network.sx.cpp -I ../

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create account
cleos create account eosio network.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract network.sx . network.sx.wasm network.sx.abi
cleos set account permission network.sx active --add-code

# test
cleos -v transfer myaccount network.sx "1.0000 EOS" "USDT"
cleos -v transfer myaccount network.sx "1.0000 USDT" "A"