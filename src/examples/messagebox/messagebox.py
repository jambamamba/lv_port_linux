import os
import sys
import lele

def buttonClicked(event):
   print(f"@@@py: button clicked, event:{event}, event.object.id:{event.object.id}")
   msgbox = event.object
   btn = msgbox.getButtonClicked()
   print(f"@@@py btn clicked: {btn.id}, {btn.getText()}")

res = lele.loadConfig("messagebox.json")
if res:
   msgbox = lele.getObjectById("/messagebox")
   res = msgbox.addEventHandler(lambda event: buttonClicked(event))

while lele.handleEvents():
   pass
