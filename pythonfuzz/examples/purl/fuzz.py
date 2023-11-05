from pythonfuzz.main import PythonFuzz
from purl import URL

@PythonFuzz
def fuzz(buf):
	try:
		string = buf.decode("ascii")
		u = URL(string)
		u.as_string()
	except UnicodeDecodeError:
		pass

if __name__ == '__main__':
	fuzz()


