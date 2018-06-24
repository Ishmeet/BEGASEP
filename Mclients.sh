#!/bin/bash
TRY=0
while (( $TRY < 40)); do
    echo "Client: $TRY"
    ./client 192.168.121.128 2222 &
    (( TRY = $TRY + 1))
done

