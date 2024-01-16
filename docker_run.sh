#!/bin/bash
sudo docker run --name vyper_fuzz_ctnr --rm -v $PWD/out -ti vyper_fuzz ./run.sh
