import sys
print(sys.path)
import lele

print("hello from python")
print(f"lele version: {lele.version()}")

def callback(str):
	print(str)

res = lele.foo(
        10,
        "hello",
        ["/home", "/tmp"], 
        {"flags":"bar", "sue": "sal"},
        lambda percent: callback(percent))
print(f"foo result:{res}")

res = lele.loadConfig("/repos/lv_port_linux/src/configs/imageview.json")
print(f"loadConfig result:{res}")

while lele.handleEvents():
    pass



