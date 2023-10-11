from boa_ancient.interpret import load, load_inline
import sys

# turn off tracebacks if we are in repl
# https://stackoverflow.com/a/64523765
if hasattr(sys, 'ps1'):
    sys.tracebacklimit = 0
