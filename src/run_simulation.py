from ctypes import CDLL

path = "C:/Users/Chris/SIRQD-epidemic-modelling/cmake-build-debug/libmodeling_library.dll"
lib = CDLL(path)
print(lib)
