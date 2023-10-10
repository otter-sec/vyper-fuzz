from vyper.compiler.phases import CompilerData

from boa.interpret.contract import VyperContract

def load_inline(program_str: str) -> VyperContract:
    data = CompilerData(program_str)

    return VyperContract(data.global_ctx)

def load(filename: str) -> VyperContract:
    with open(filename) as f:
        data = CompilerData(f.read())

    return VyperContract(data.global_ctx)
