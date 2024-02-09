@external
@view
def list_test() -> bool:
    return [1, 2, 3][0] == 1

@external
@view
def tuple_test() -> bool:
    x: uint256 = 1
    y: uint256 = 2
    z: uint256 = 3
    return (x, y, z)[0] == 1

@external
@view
def bitwise_and_test() -> bool:
    return bitwise_and(31337, 8008135) == 12353

@external
@view
def bitwise_not_test() -> bool:
    return bitwise_not(0) == 115792089237316195423570985008687907853269984665640564039457584007913129639935

@external
@view
def bitwise_or_test() -> bool:
    return bitwise_or(31337, 8008135) == 8027119

@external
@view
def bitwise_xor_test() -> bool:
    return bitwise_xor(31337, 8008135) == 8014766

@external
@view
def shift_test() -> bool:
    return shift(2, 8) == 512


@external
@view
def ecadd_test() -> bool:
    P: uint256[2] = [ 1, 2 ]
    T: uint256[2] = [ 1368015179489954701390400359078579693043519447331113978918064868415326638035, 9918110051302171585080402603319702774565515993150576347155970296011118125764 ]
    return ecadd(P, P)[0] == T[0] and ecadd(P, P)[1] == T[1]

@external
@view
def ecmult_test() -> bool:
    P: uint256[2] = [ 1, 2 ]
    T: uint256[2] = [ 1368015179489954701390400359078579693043519447331113978918064868415326638035, 9918110051302171585080402603319702774565515993150576347155970296011118125764 ]
    return ecmul(P, 2)[0] == T[0] and ecmul(P, 2)[1] == T[1]

@external
@view
def sha256_test() -> bool:
    return sha256(b"potato") == 0xe91c254ad58860a02c788dfb5c1a65d6a8846ab1dc649631c7db16fef4af2dec

@external
@view
def concat_test() -> bool:
    return concat("why", " ", "hello", " ", "there", "!") == "why hello there!"

@external
@view
def convert_and_slice_test() -> bool:
    return slice(convert(123, bytes32), 31, 1) == b"{"

@external
@view
def uint2str_test() -> bool:
    return uint2str(123) == "123"

@external
@view
def abs_test() -> bool:
    return abs(-1) == 1

@external
@view
def ceil_test() -> bool:
    return ceil(1.5) == 2

@external
@view
def floor_test() -> bool:
    return floor(1.5) == 1

@external
@view
def max_test() -> bool:
    return max(1,2) == 2


@external
@view
def min_test() -> bool:
    return min(1,2) == 1


@external
@view
def pow_mod256_test() -> bool:
    return pow_mod256(100, 100) == 59041770658110225754900818312084884949620587934026984283048776718299468660736


@external
@view
def sqrt_test() -> bool:
    return sqrt(9.0) == 3.0

@external
@view
def uint256_addmod_test() -> bool:
    return uint256_addmod(6, 13, 8) == 3

@external
@view
def uint256_mulmod_test() -> bool:
    return uint256_mulmod(11, 2, 5) == 2

# no unsafe_add test as its placeholder
# no unsafe_sub test as its placeholder
# no unsafe_mul test as its placeholder
# no unsafe_div test as its placeholder

@external
@view
def as_weit_test() -> bool:
    return as_wei_value(1.337, "ether") == 1337000000000000000

@external
@view
def len_test() -> bool:
    return len("hello") == 5

@external
@view
def print_test() -> bool:
    print("print_test = True")
    return True

@external
@view
def keccak256_test() -> bool:
    return keccak256(b"potato") == 0x9e159dfcfe557cc1ca6c716e87af98fdcb94cd8c832386d0429b2b7bec02754f

@external
@view
def method_id_test() -> bool:
    expected: Bytes[4] = b'\xa9\x05\x9c\xbb'
    return method_id("transfer(address,uint256)") == expected

@external
@view
def _abi_encode_test() -> bool:
    x: uint256 = 1
    return _abi_encode(x) == b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01'

@external
@view
def empty_test() -> bool:
    return empty(uint256) == 0

@external
@view
def _abi_decode_test() -> bool:
    x: uint256 = empty(uint256)
    y: uint256 = empty(uint256)
    x,y  = _abi_decode(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02', (uint256,uint256))
    z: uint256 = empty(uint256)
    z  = _abi_decode(b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03', uint256)
    return x == 1 and y == 2 and z == 3