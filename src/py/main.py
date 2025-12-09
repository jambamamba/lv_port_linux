import sys
import lele


print("@@@py hello world from python")
print(f"@@@py lele.version(): {lele.version()}")

res = lele.loadConfig("/repos/lv_port_linux/src/configs/testview.json")
print(f"@@@py loadConfig result:{res}")

obj = lele.getObjectById("/views:0/view/label")
user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
print(user_attributes)

while lele.handleEvents():
   pass
