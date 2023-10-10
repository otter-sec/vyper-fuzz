#!/bin/bash

sudo docker run --name vyper_fuzz_ctnr --rm -v $PWD/out vyper_fuzz
