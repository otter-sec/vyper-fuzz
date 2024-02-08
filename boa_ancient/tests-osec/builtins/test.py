from sys import path
from os.path import join, dirname, abspath
from os import listdir
path.append(abspath(dirname(dirname(dirname(dirname(__file__))))))
import boa_ancient as boa
fname = join(dirname(__file__))
c = boa.load(join(dirname(__file__), [x for x in listdir(fname) if x.endswith('.vy')][0]))

for x in dir(c):
    if 'test' in x:
        if 'ec' in x:
            try:
                import ecdsa
            except:
                continue
        tst = getattr(c, x)
        if callable(tst):
            print(f"{x} = {tst().value}")