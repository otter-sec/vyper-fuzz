@internal
def test() -> uint256:
    return 1
@internal
def func_name(a1: int256,a2: String[1337],a3: int256) -> String[3]:
    a: uint256 = self.test()
    return "12"
@external
def main() -> int256:
    i: int256 = 22
    a: String[4] = self.func_name(i, "aaaa", -22)
    return i
