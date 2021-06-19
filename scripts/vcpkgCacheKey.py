from vcpkg.depList import getCommonList
from vcpkgServer.depList import getServerList
import hashlib
import sys
from itertools import chain

if __name__ == "__main__":
    print(hashlib.md5("".join(chain(getCommonList(sys.argv[1:]), getServerList(sys.argv[1:]))).encode()).hexdigest())