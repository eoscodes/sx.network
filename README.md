# `network.sx`

> SX Network - Liquidity pool aggregator

The network pool aggregator smart contract will auto-detect which liquidity pool offers the highest return rate for the incoming & outgoing trade pair.

## Quickstart

Conviently convert EOS <> USDT

```bash
$ cleos transfer myaccount network.sx "1.0000 EOS" "USDT"
$ cleos transfer myaccount network.sx "2.5000 USDT" "EOS" --contract tethertether
```

## Build

```
$ eosio-cpp network.sx.cpp -I ../
```
