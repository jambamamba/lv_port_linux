import os
import sys
import lele

def sliderEventHandler(event):
   print(f"@@@py sliderEventHandler id:{event.id}, code:{event.code}, value:{event.value}")
   user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
   print(user_attributes)
   # if event.code == lele.Event().Code.EVENT_VALUE_CHANGED:
   #       print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
   #       obj = lele.getObjectById("/view/with_bg_img")
   #       style = obj.getStyle("img")
   #       dic = style.getValue(["background/size"])
   #       print(f"@@@py style.dic3 {dic}")
   #       style.setValue({"background/size":str(event.value)})

   #       obj = lele.getObjectById("scale-label")
   #       obj.setText(f"Scale: {event.value}%")

res = lele.loadConfig("hello-world-flags.json")
if not res:
   sys.exit(1)

lele.getObjectById("/slider").addEventHandler(lambda event: sliderEventHandler(event))
# lele.addEventHandler("/slider", lambda event: sliderEventHandler(event))

while lele.handleEvents():
   pass
