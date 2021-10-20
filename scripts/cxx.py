import sys

if __name__ == "__main__":
    print({"macos":"/usr/local/opt/llvm/bin/clang++", "ubuntu":"g++-10", "windows":"cl"}[sys.argv[1]])