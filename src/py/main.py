import sys
print(sys.path)
import traceback
import lele


print("@@@py hello from python")
print(f"@@@py lele.version(): {lele.version()}")

# event = lele.Event()
# print(f"event.id: {event.event_id}")
# print(f"===========================================")

def callback(str):
   print(f"@@@py str:'{str}'")

def sliderMoved(event):
   if event.code == event.Type.ValueChanged.value:
        print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
   pass

def pushButtonPressed(event):
   # print(f"@@@py event: {event}")
   # user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
   # print(user_attributes)
   # print(f"@@@py event.object: {event.object}")
   # user_attributes = [attr for attr in dir(event.object) if not attr.startswith('__')]
   # print(user_attributes)
   # for event_type in event.Type:
   #    print(f"@@@py enum event.Type: Name: {event_type.name}, Value: {event_type.value}")
   if event.code == event.Type.Clicked.value:
      print(f"@@@py event: {event}")
      user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
      print(user_attributes)
      print(f"@@@py button pressed, event.object: {event.object}")
      user_attributes = [attr for attr in dir(event.object) if not attr.startswith('__')]
      print(user_attributes)
      print(f"@@@py event.object.getText(): {event.object.getText()}")
      print(f"@@@py event.object: {event.object.id}")
      obj = lele.getObjectById("/views:0/view/label")
      print(f"@@@py getObjectById {obj}, obj.getText() {obj.getText()}")
      obj.setText("Cocoa bean!")
   pass

res = lele.foo(
        10,
        "hello",
        ["/home", "/tmp"], 
        {"flags":"bar", "sue": "sal"},
        lambda percent: callback(percent))
print(f"@@@py foo result:{res}")

res = lele.loadConfig("/repos/lv_port_linux/src/configs/imageview.json")
print(f"@@@py loadConfig result:{res}")

lele.addEventHandler("slider0", lambda event: sliderMoved(event))
lele.addEventHandler("push_button0", lambda event: pushButtonPressed(event))

obj = lele.getObjectById("/views:0/view/label")
user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
obj = lele.getObjectById("push_button0")
for button_type in obj.Type:
    print(f"@@@py enum button.Type: Name: {button_type.name}, Value: {button_type.value}")

try:
   while lele.handleEvents():
      pass
except:
   print(traceback.format_exc())
