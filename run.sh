#!/bin/bash

#sleep 1337
py-afl-fuzz -i in -o out -- ./harness.py
