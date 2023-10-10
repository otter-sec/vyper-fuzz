#!/usr/bin/env python3
import afl
from boa import loads

# TODO: Create some class that describes the program state for a vyper contract

# Get Vyper contract from stdin
def read_prog():
    program = ""
    while True:
        try:
            line = input()
        except EOFError:
            break
        program += line + "\n"
    return program

afl.init()
program = read_prog()
print(program)

# Run against interpreter
interpreted = loads(program)
print(f"interpreted: {interpreted.main()}")

from vyper import compile_codes

# Try to pass compilation
compiled = compile_codes({"contract": program})

# TODO: Run compiled code

# TODO: Differential analysis