from boa_ancient.interpret.object import VyperObject
from vyper.codegen.types.types import BaseType, is_integer_type, ByteArrayType, StringType, DArrayType, SArrayType, MappingType

class BuiltinFunction:
    _id: str

class Empty(BuiltinFunction):
    _id = "empty"

    def eval(self, context, type_expr):
        typ = get_type_from_annotation(type_expr)
        return VyperObject.empty(new_type_to_old_type(typ))

class Concat(BuiltinFunction):
    _id: "concat"

    def eval(self, context, *args):
        result = ""
        for i in args:
            result += i.value
        return VyperObject(result, typ=f"String[{len(result)}]")        

DISPATCH_INTERNAL = {
    "empty": Empty(),
    "concat": Concat(),
}
