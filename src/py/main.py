import lele
import sys

def main():
    print("hello from python")
    print(f"lele version: {lele.version()}")

    res = lele.foo(
          10,
          "hello",
          ["/home", "/tmp"], 
          {"flags":"bar", "sue": "sal"},
          lambda percent: callback(percent))
    print(f"foo result:{res}")
	
    res = lele.loadConfig("imageview.json")
    print(f"loadConfig result:{res}")

    res = lele.runMainLoop() #osm todo: pass callback, that should return true to continue main loop, or false to exit
    print(f"loadConfig result:{res}")

    return 0


def callback(str):
	print(str)
