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

    while lele.handleEvents():
        pass


def callback(str):
	print(str)

