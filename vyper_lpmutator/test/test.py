import sys
sys.path.append("build/")
from VyperFuzzLib import VyperFuzz

fuzzer = VyperFuzz()

with open(sys.argv[1], "rb") as f:
	data = f.read()

print(fuzzer.Mutate(bytes(data), 400))
