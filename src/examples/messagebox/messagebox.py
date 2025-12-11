import os
import sys
import lele
import traceback

def buttonClicked(event):
   print(f"@@@py: button clicked, event:{event}, event.object.id:{event.object.id}")
   msgbox = event.object
   btn = msgbox.getButtonClicked()
   print(f"@@@py btn clicked: {btn.id}, {btn.getText()}")
   if btn.id == "push2":
      return True
   return False

res = lele.loadConfig("messagebox.json")
if res:
   msgbox = lele.getObjectById("/messagebox")
   res = msgbox.addEventHandler(lambda event: buttonClicked(event))

try:
   while lele.handleEvents():
      pass
except:
   print(traceback.format_exc())
