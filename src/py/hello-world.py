import sys
import lele


print("@@@py hello world from python")
print(f"@@@py lele.version(): {lele.version()}")

res = lele.loadConfig("/repos/lv_port_linux/src/configs/hello-world.json")
print(f"@@@py loadConfig result:{res}")

obj = lele.getObjectById("/hello-world/label")
print(f"@@@py obj: {obj}")
user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
print(user_attributes)

while lele.handleEvents():
   pass
