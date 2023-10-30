@external
def func_name(a1: uint256,a2: String,a3: int256) -> uint256:
    a: uint256 = 1234
    b: String[2] = "ab"
    concat(b, "c")
    return a

def test() -> int64:
    return 1 + 2 * 3 % 4

@external
def anotherfunc(a: int64,b: uint256,qaaaaa: bytes32,c: uint256) -> uint256:
    xxx: String[64] = "Hellooou"
    return xxx
