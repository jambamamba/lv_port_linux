import os
import sys
import lele

res = lele.loadConfig("hello-world.json")
if res:
   label = lele.getObjectById("/hello-world/label")
   text = label.getText()
   version = f"{lele.version()['Major']}.{lele.version()['Minor']}"
   label.setText(f"{text}\n\nLele Version: {version}")

while lele.handleEvents():
   pass
