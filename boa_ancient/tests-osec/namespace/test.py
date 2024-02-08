from sys import path
from os.path import join, dirname, abspath
path.append(abspath(dirname(dirname(dirname(dirname(__file__))))))
import boa_ancient as boa
c = boa.load(join(dirname(__file__), "namespace.vy"))

print(c.main())