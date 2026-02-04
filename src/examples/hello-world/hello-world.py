import os
import sys
import lele

res = lele.loadConfig("hello-world.json")
if res:
   label_hello_world = lele.getObjectById("/hello-world/label")
   print(f"label text: {label_hello_world}")

   label_version = lele.getObjectById("/hello-world/version")
   version = f"{lele.version()['Major']}.{lele.version()['Minor']}"
   label_version.setText(f"Lele-UI Version: {version}")

while lele.handleEvents():
   pass
