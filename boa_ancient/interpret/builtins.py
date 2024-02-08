from boa_ancient.interpret.object import VyperObject
from vyper.codegen.types.types import BaseType, is_integer_type, ByteArrayType, StringType, DArrayType, SArrayType, MappingType
import warnings
try:
    from ecdsa.ecdsa import ellipticcurve
    ecdsa_installed = True
    curve = ellipticcurve.CurveFp(0x2523648240000001BA344D80000000086121000000000013A700000000000013, 0, 2)

    def point(x, y, curve=curve):
        return ellipticcurve.Point(curve, x, y)
    generator = point(0x2523648240000001BA344D80000000086121000000000013A700000000000012, 1)
except:
    warnings.warn("ecdsa (see pypi) not installed, EC funcs will not work")
    ecdsa_installed = False

class BuiltinFunction:
    _id: str

class Empty(BuiltinFunction):
    _id = "empty"

    def eval(self, context, type_expr):
        typ = get_type_from_annotation(type_expr)
        return VyperObject.empty(new_type_to_old_type(typ))


class BitWiseAnd(BuiltinFunction):
    _id = "bitwise_and"

    def eval(self, context, *args):
        result = args[0].value
        for i in args[1:]:
            result &= i.value
        return VyperObject(result, typ="uint256")

class BitWiseNot(BuiltinFunction):
    _id = "bitwise_not"

    def eval(self, context, *args):
        return VyperObject(~args[0].value & 2**256, typ="uint256") # Masking to ensure uint

class BitWiseOr(BuiltinFunction):
    _id = "bitwise_or"

    def eval(self, context, *args):
        result = args[0].value
        for i in args[1:]:
            result |= i.value
        return VyperObject(result, typ="uint256")

class BitWiseXor(BuiltinFunction):
    _id = "bitwise_xor"

    def eval(self, context, *args):
        result = args[0].value
        for i in args[1:]:
            result ^= i.value
        return VyperObject(result, typ="uint256")


class Shift(BuiltinFunction):
    _id = "shift"

    def eval(self, context, *args):
        # note we need to be able to handle unsigned and signed shifts
        val = args[0].value
        shift = args[1].value
        # TODO: will deal with edge cases later 
        return VyperObject(val << shift, typ="uint256")

class ECADD(BuiltinFunction):
    _id = "ecadd"
    def eval(self, context, *args):
        p_1 = point(args[0].value[0].value, args[0].value[1].value)
        p_2 = point(args[1].value[0].value, args[1].value[1].value)
        p_3 = p_1+p_2
        return VyperObject([VyperObject(p_3.x(), typ="uint256"), VyperObject(p_3.y(), typ="uint256")], typ="uint256[2]")

class ECMUL(BuiltinFunction):
    _id = "ecmul"
    def eval(self, context, *args):
        p_1 = point(args[0].value[0].value, args[0].value[1].value)
        p_2 = args[1].value
        p_3 = p_1*p_2
        return VyperObject([VyperObject(p_3.x(), typ="uint256"), VyperObject(p_3.y(), typ="uint256")], typ="uint256[2]")

# TODO: ecrecover, keccak256

class SHA256(BuiltinFunction):
    _id = "sha256"

    def eval(self, context, *args):
        from hashlib import sha256
        h = sha256()
        for i in args:
            h.update(i.value)
        return VyperObject(h.digest(), typ="bytes32")


class Concat(BuiltinFunction):
    _id = "concat"

    def eval(self, context, *args):
        result = ""
        for i in args:
            result += i.value
        return VyperObject(result, typ=f"String[{len(result)}]")  

int128_to_uint256 = lambda x: x & 2**128-1
bool_to_uint256 = lambda x: int(x)
class Conversions:
    class uint:
        # TODO: check these
        uint_to_int = lambda x: x
        uint_to_bool = lambda x: bool(x)
        uint_to_decimal = lambda x: float(x)
        uint_to_bytes = lambda x: x.to_bytes(32, "big")
    class bool:
        bool_to_uint = lambda x: int(x)
        bool_to_decimal = lambda x: float(x)
        bool_to_bytes = lambda x: b"\x01" if x else b"\x00"
        bool_to_int = lambda x: int(x)
    class decimal:
        decimal_to_uint = lambda x: int(x)
        decimal_to_bool = lambda x: bool(x)
        decimal_to_bytes = lambda x: int(x).to_bytes(32, "big")
        decimal_to_int = lambda x: int(x)
    class bytes:
        bytes_to_uint = lambda x: int.from_bytes(x, "big")
        bytes_to_bool = lambda x: bool(int.from_bytes(x, "big"))
        bytes_to_decimal = lambda x: float(int.from_bytes(x, "big"))
        bytes_to_int = lambda x: int.from_bytes(x, "big")
    class int:
        int_to_uint = lambda x: x & 2**128-1
        int_to_bool = lambda x: bool(x)
        int_to_decimal = lambda x: float(x)
        int_to_bytes = lambda x: x.to_bytes(32, "big")

class Convert(BuiltinFunction):
    _id = "convert"
    # TODO: support bytes1 and non-expected lengths
    def eval(self, context, *args):
        from_type = args[0].typ.__repr__().split("[")[0].strip("][1234567890")
        to_type = args[1].__repr__().split("(")[-1][:-1].strip("][1234567890")
        cls = getattr(Conversions, f"{from_type}", None)
        if cls is None:
            raise Exception(f"Conversion attempt from {from_type} but is not supported")
        conversion = getattr(cls, f"{from_type}_to_{to_type}", None)
        if conversion is None:
            raise Exception(f"Conversion from {from_type} to {to_type} is not supported")
        return VyperObject(conversion(args[0].value), typ=args[1].__repr__())

class Slice(BuiltinFunction):
    _id = "slice"
    def eval(self, context, *args):
        return VyperObject(args[0].value[args[1].value:args[1].value+args[2].value], typ=args[0].typ)

class Uint2Str(BuiltinFunction):
    _id = "uint2str"
    def eval(self, context, *args):
        return VyperObject(str(args[0].value), typ="String")

class Extract32(BuiltinFunction):
    # TODO: support other conversion types
    _id = "extract32"
    def eval(self, context, *args):
        start_idx = (len(args[0].value) - args[1].value - 32)
        data = args[0].value[start_idx:start_idx+32]
        to_typ = "bytes32"
        if len(args) == 3:
            to_typ = args[2].__repr__().split("(")[-1][:-1]
            if to_typ in ["uint256", "int128"]:
                data = int.from_bytes(data, "big")
            elif to_typ in ["address"]:
                data = hex(int.from_bytes(data, "big")).upper()
        return VyperObject(data, typ=to_typ)



"""
DISPATCH_INTERNAL = {
    "empty": Empty(),
    "concat": Concat(),
}
"""
DISPATCH_INTERNAL = {}
# just a bit of janky code because im too lazy to update this table by hand
for x in BuiltinFunction.__subclasses__():
    DISPATCH_INTERNAL[x._id] = x()
