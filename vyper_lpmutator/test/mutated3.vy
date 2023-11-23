@internal
def bar() -> int256:
    d: String[12] = "vyper"
    c: String[1337] = concat("CCCCCCCCCCCCCCCCCCCCCCCCCC","AAAAAAAAAAA", "")
    return 0

@external
def main() -> int256:
    a: int256 = 10
    b: int256 = self.bar()
    return a

