import os
import sys
import lele

res = lele.loadConfig("hello-world-flags.json")
if not res:
   sys.exit(1)

while lele.handleEvents():
   pass
