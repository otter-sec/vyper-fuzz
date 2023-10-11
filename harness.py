#!/usr/bin/env python3
import afl
import os
import subprocess
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
        interpreted = subprocess.run(
            ["python3", "interpret.py", program],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT
            ).stdout.decode().split(":")[1].strip()
        if interpreted == "InterpretationFailed":
            os._exit(0)
        print(f"interpreted: {interpreted}")

        # Switch to venv
        exec(open(activate_venv_file).read(), {'__file__': activate_venv_file})
        import boa

        # Run against compiler
        compiled = str(boa.loads(program).main())
    except:
        os._exit(0)
    print(f"compiled: {compiled}")

    # TODO: Differential analysis
    if interpreted != compiled:
        raise Exception(f"Inconsistency:\n interpreted = {interpreted}; compiled = {compiled}")

if __name__ == "__main__":
    main()