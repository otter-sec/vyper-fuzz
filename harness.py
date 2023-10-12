#!/usr/bin/env python3
import afl
import os
import subprocess
import time
activate_venv_file = "./venv/bin/activate_this.py"

# TODO: Create some class that describes the program state for a vyper contract

def read_prog():
    program = ""
    while True:
        try:
            line = input()
        except EOFError:
            break
        except UnicodeDecodeError:
            os._exit(0)
        program += line + "\n"
    return program

def main():
    # Hook stdin with fuzzer and start instrumentation
    afl.init()

    # Get Vyper contract from afl
    program = read_prog()
    print(program)

    # Run against interpreter
    # Temporarily using subprocess for 2 reasons:
    # - untrack coverage from the interpreter
    # - facilitate isolating the 2 titanboa versions
    try:
        start = time.time()
        interpreted = subprocess.run(
            ["python3", "interpret.py", program],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT
            ).stdout.decode().split(":")[1].strip()
        if interpreted == "InterpretationFailed":
            os._exit(0)
        end = time.time()
        print(f"time to interpret: {end - start}")
        print(f"interpreted: {interpreted}")

        # Switch to venv
        exec(open(activate_venv_file).read(), {'__file__': activate_venv_file})
        start = time.time()
        import boa
        end = time.time()
        print(f"time to load boa: {end - start}")

        # Run against compiler
        start = time.time()
        compiled = str(boa.loads(program).main())
        end = time.time()
        print(f"time to compile and run: {end - start}")
    except:
        os._exit(0)
    print(f"compiled: {compiled}")

    # TODO: Differential analysis
    if interpreted != compiled:
        raise Exception(f"Inconsistency:\n interpreted = {interpreted}; compiled = {compiled}")

if __name__ == "__main__":
    main()