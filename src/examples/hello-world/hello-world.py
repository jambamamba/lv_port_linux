import os
import sys
import lele

script_dir = os.path.dirname(os.path.realpath(__file__))
res = lele.loadConfig(f"{script_dir}/hello-world.json")
if res:
   label = lele.getObjectById("/hello-world/label")
   text = label.getText()
   version = f"{lele.version()['Major']}.{lele.version()['Minor']}"
   label.setText(f"{text}\n\nLele Version: {version}")

while lele.handleEvents():
   pass
