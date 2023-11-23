import collections
import sys

prev_line = 0
prev_filename = ''
data = collections.defaultdict(set)
fail_case = False

def trace(frame, event, arg):
    if event != 'line':
        return trace

    global prev_line
    global prev_filename

    func_filename = frame.f_code.co_filename
    func_line_no = frame.f_lineno

    if func_filename != prev_filename:
        # We need a way to keep track of inter-files transferts,
        # and since we don't really care about the details of the coverage,
        # concatenating the two filenames in enough.
        data[func_filename + prev_filename].add((prev_line, func_line_no))
    else:
        data[func_filename].add((prev_line, func_line_no))

    prev_line = func_line_no
    prev_filename = func_filename

    return trace

def set_fail():
    global fail_case
    if not fail_case:
        fail_case = True

def get_coverage():
    global fail_case
    if fail_case:
        fail_case = False
        return 0
    return sum(map(len, data.values()))
