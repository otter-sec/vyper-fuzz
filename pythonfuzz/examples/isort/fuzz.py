from pythonfuzz.main import PythonFuzz
import isort

@PythonFuzz
def fuzz(buf):
	try:
		string = buf.decode("ascii")
		sorted_code = isort.code(string)
	except UnicodeDecodeError:
		pass

if __name__ == '__main__':
	fuzz()
