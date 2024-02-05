#!/bin/bash

apt install -y python3 python-is-python3 git python3-pip virtualenv

cd vyper_lpmutator
./install.sh
cd ..

virtualenv venv
. venv/bin/activate
pip install -r requirements.txt
cd pythonfuzz && python3 setup.py install

#git clone https://github.com/vyperlang/vyper.git
#cd vyper && git apply ../asan.patch && python3 setup.py install

