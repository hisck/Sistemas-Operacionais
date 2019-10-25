#!/bin/bash

screen -dmSL server1 ./server/server1.out
screen -dmSL server2 ./server/server2.out
screen -dmSL server3 ./server/server3.out
sleep 2
screen -dmSL proxy ./proxy/proxy.out
sleep 2
./client/client.out