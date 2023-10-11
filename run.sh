#!/bin/bash

#sleep 1337
py-afl-fuzz -i in -o out -t 10000 -- ./harness.py
