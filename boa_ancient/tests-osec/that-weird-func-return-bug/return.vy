@internal
def bar() -> String[90]:
    lsfq: String[2097158] = "BBBBBBAAAAAAAAWWWWWWWW"
    ccx: uint256 = 9223372036854775807
    yyy: int256 = 12
    gdag: String[90] = "U"
    return gdag

@internal
def foo() -> String[90]:
    aad: bytes32 = 0x4141414141414141414141414141414141414141414141414141414141414141
    b: String[90] = self.bar()
    return b

@external
def main() -> String[2147483738]:
    return self.foo()