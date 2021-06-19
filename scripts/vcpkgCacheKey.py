from vcpkg.depList import getCommonList
from vcpkg.depList import getServerList
import hashlib
import sys

if __name__ == "__main__":
    print(hashlib.md5("".join(chain(getCommonList(sys.argv[1:]), getServerList(sys.argv[1:]))).encode()).hexdigest())