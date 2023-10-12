#!/bin/bash

sudo apt install -y python3 python-is-python3 git python3-pip virtualenv
virtualenv venv
. venv/bin/activate
pip install -r requirements.txt
cd pythonfuzz && python3 setup.py install
