import os
import sys
import lele

print("@@@py hello world from python")
print(f"@@@py lele.version(): {lele.version()}")

script_dir = os.path.dirname(os.path.realpath(__file__))
res = lele.loadConfig(f"{script_dir}/hello-world.json")
print(f"@@@py loadConfig result:{res}")

obj = lele.getObjectById("/hello-world/label")
print(f"@@@py obj: {obj}")

while lele.handleEvents():
   pass
