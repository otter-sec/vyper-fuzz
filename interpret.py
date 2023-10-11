from sys import argv
from boa_ancient import loads

contract = loads(argv[1])

try:
    result = contract.main()
except:
    result = "InterpretationFailed"
print(result)
