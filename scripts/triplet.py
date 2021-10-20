import sys

if __name__ == "__main__":
    print({"macos":"osx", "ubuntu":"linux", "windows":"windows"}[sys.argv[1]])