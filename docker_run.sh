#!/bin/bash
sudo docker run --name vyper_fuzz_ctnr --rm -v $PWD/corpus -v $PWD/crashes -ti vyper_fuzz ./run.sh
