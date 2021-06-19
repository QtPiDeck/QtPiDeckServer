from vcpkg.depList import getCommonList
from vcpkgServer.depList import getServerList
import sys
from itertools import chain

if __name__ == "__main__":
    res = chain(getCommonList(sys.argv[1:]), getServerList(sys.argv[1:]))
    print(" ".join(res) if res else "nil") # there is no package with such name, right?