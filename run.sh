#!/bin/sh

./build.sh
echo "Starting app"
sleep 1
clear
cd build
./P11_Mail_Server_run
