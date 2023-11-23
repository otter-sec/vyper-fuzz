@internal
def bar() -> int256:
    lsfq: String[6] = "BBBBBB"
    yyy: int256 = -12
    gdag: String[90] = concat("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC", "DDDDDD")
    return yyy

@internal
def foo() -> bytes32:
    aad: bytes32 = 0x4141414141414141414141414141414141414141414141414141414141414141
    b: int256 = self.bar()
    return aad

@external
def main() -> bytes32:
    return self.foo()

