import sys
print(sys.path)
import lele

print("@@@py hello from python")
print(f"@@@py lele.version(): {lele.version()}")

# event = lele.Event()
# print(f"event.id: {event.event_id}")
# print(f"===========================================")

def callback(str):
   print(f"@@@py str:'{str}'")

def sliderMoved(event):
   if event.code == 35: #value changed
        print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.type}, event.action: {event.action}")
   pass

def pushButtonPressed(event):
   if event.code == 10: #clicked
      print(f"@@@py button pressed: {event.object.id}, event.type: {event.type}, event.action: {event.action}")
      obj = lele.getObjectById("/views:0/view/label")
      print(f"@@@py getObjectById {obj}, obj.getText() {obj.getText()}")
      obj.setText("Choco bean!")
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
print(f"@@@py obj: {obj}")
obj = lele.getObjectById("push_button0")
print(f"@@@py obj: {obj}")
for button_type in obj.Type:
    print(f"@@@py enum button.Type: Name: {button_type.name}, Value: {button_type.value}")

while lele.handleEvents():
    pass



