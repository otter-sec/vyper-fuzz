import boa_ancient as boa
import vyper
from vyper.exceptions import VyperException, ParserException
from eth_abi import abi
from eth_abi.exceptions import InsufficientDataBytes
from helpers.environment import Env
from pythonfuzz.main import PythonFuzz
from pythonfuzz.tracer import set_fail

from eth_utils import decode_hex, to_checksum_address

MAIN_CALL_DATA = decode_hex("dffeadd0") # main function selector

env = Env()

def get_bytecode(odict, name):
    for item in odict:
        if item[0] == name:
            return item[1]["bytecode"]

@PythonFuzz
def fuzz(buf):
    # typ = "int256"

    # try:
    #     program = buf.decode("ascii")
        
    #     vcontract = vyper.compile_codes({"0":program}).items()
    #     deploy_bytecode = get_bytecode(vcontract, "0")

    #     addr, _ = env.deploy_code(bytecode=decode_hex(deploy_bytecode))
    #     contract_addr = to_checksum_address(addr)
    
    #     computation = env.execute_code(
    #         to_address=contract_addr,
    #         data=MAIN_CALL_DATA
    #     )

    #     compiled, = abi.decode([typ], computation.output)
    # except:
    #     return

    # compiled_str = f"{typ}:{compiled}"

    # interpreted_str = str(boa.loads(program).main())

    # if interpreted_str != compiled_str:
    #     raise Exception(f"interpreted: {interpreted_str} != compiled: {compiled_str}")
    try:
        program = buf.decode("ascii")

        typ = program.split("main() -> ")[1].split(":")[0]
        if not typ:
            return

        # coverage = sys.settrace(tracer.trace)
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
        # print(compiled_str)

        interpreted_str = str(boa.loads(program).main())
        # print(interpreted_str)
    except:
        set_fail()
        return

    if interpreted_str != compiled_str:
        raise Exception(f"interpreted: {interpreted_str} != compiled: {compiled_str}")
if __name__ == "__main__":
    fuzz()
