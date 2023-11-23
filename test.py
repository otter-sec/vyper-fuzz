#!/usr/bin/env python3
import boa_ancient as boa
import vyper
from eth_abi import abi
from helpers.environment import Env

from eth_utils import decode_hex, to_checksum_address

MAIN_CALL_DATA = decode_hex("0xdffeadd0") # main function selector

env = Env()

def read_prog():
    program = ""
    while True:
        try:
            line = input()
        except EOFError:
            break
        program += line + "\n"
    return program

def get_bytecode(odict, name):
    for item in odict:
        if item[0] == name:
            return item[1]["bytecode"]


def fuzz(buf):
    program = buf

    typ = program.split("main() -> ")[1].split(":")[0]
    if not typ:
        return

    vcontract = vyper.compile_codes({"0":program}).items()
    deploy_bytecode = get_bytecode(vcontract, "0")

    addr, _ = env.deploy_code(bytecode=decode_hex(deploy_bytecode))
    contract_addr = to_checksum_address(addr)

    computation = env.execute_code(
        to_address=contract_addr,
        data=MAIN_CALL_DATA
    )

    compiled, = abi.decode([typ], computation.output)
    if(typ == "bytes32"):
        compiled = int(compiled.hex(), 16)
    compiled_str = f"{typ}:{compiled}"
    print(compiled_str)

    interpreted_str = str(boa.loads(program).main())
    print(interpreted_str)

    if interpreted_str != compiled_str:
        raise Exception(f"interpreted: {interpreted_str} != compiled: {compiled_str}")

    

if __name__ == "__main__":
    fuzz(read_prog())
