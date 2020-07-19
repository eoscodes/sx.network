# `network.sx`

> SX Network - Liquidity pool aggregator 

## Quickstart

Quickly convert to any SX pool at the highest rate

```bash
$ cleos transfer myaccount network.sx "1.0000 EOS" "USDT"
$ cleos transfer myaccount network.sx "2.5000 USDT" "EOS" --contract tethertether
```

## Build

```
$ eosio-cpp network.sx.cpp -I ../
```
