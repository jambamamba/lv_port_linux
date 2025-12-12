import os
import sys
import lele
import traceback

print(f"@@@py================================")
print(f"@@@py lele.Event:{lele.Event}")
print(f"@@@py attributes: {[attr for attr in dir(lele.Event) if not attr.startswith('__')]}")
print(f"@@@py================================")
print(f"@@@py lele.Object:{lele.Object}")
print(f"@@@py attributes: {[attr for attr in dir(lele.Object) if not attr.startswith('__')]}")
print(f"@@@py================================")
print(f"@@@py lele.Label:{lele.Label}")
print(f"@@@py attributes: {[attr for attr in dir(lele.Label) if not attr.startswith('__')]}")
print(f"@@@py================================")
print(f"@@@py lele.MessageBox:{lele.MessageBox}")
print(f"@@@py attributes: {[attr for attr in dir(lele.MessageBox) if not attr.startswith('__')]}")
print(f"@@@py================================")
print(f"@@@py lele.Button:{lele.Button}")
print(f"@@@py attributes: {[attr for attr in dir(lele.Button) if not attr.startswith('__')]}")

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
