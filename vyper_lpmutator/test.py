import sys
sys.path.append("build/")
from VyperFuzzLib import VyperFuzz

fuzzer = VyperFuzz()

with open(sys.argv[1], "rb") as f:
	data = f.read()

code = fuzzer.ProtoToVyper(data).decode("utf-8")
print(code)

mutated = fuzzer.Mutate(data, 500)
mutated_code = fuzzer.ProtoToVyper(mutated).decode("utf-8")
print(mutated_code)
