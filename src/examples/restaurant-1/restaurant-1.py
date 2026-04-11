import os
import sys
import lele

res = lele.loadConfig("restaurant-1.json")
if not res:
   sys.exit(1)

while lele.handleEvents():
   pass
