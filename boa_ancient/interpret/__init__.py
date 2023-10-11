from vyper.compiler.phases import CompilerData

from boa_ancient.interpret.contract import VyperContract

def load(filename: str) -> VyperContract:
    with open(filename) as f:
        data = CompilerData(f.read())

    return VyperContract(data.global_ctx)

def loads(source_code: str) -> VyperContract:
    data = CompilerData(source_code)

    print("test")
    return VyperContract(data.global_ctx)