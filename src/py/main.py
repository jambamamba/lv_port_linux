import sys
print(sys.path)
import lele

print("hello from python")
print(f"lele.version(): {lele.version()}")

event = lele.Event()
print(f"event.id: {event.event_id}")
print(f"===========================================")

def callback(str):
   print(str)

def sliderMoved(event):
   if event.code == 35: #value changed
        print(f"@@@py slider {event.object_id} moved to: {event.value}%")
        print(f"@@@py    event.id: {event.object_id}, event.type: {event.type}, event.action: {event.action}")
   pass

def pushButtonPressed(event):
   if event.code == 10: #clicked
      print(f"@@@py button pressed: {event.object_id}")
      print(f"@@@py     event.id: {event.object_id}, event.type: {event.type}, event.action: {event.action}")
   pass

res = lele.foo(
        10,
        "hello",
        ["/home", "/tmp"], 
        {"flags":"bar", "sue": "sal"},
        lambda percent: callback(percent))
print(f"foo result:{res}")

res = lele.loadConfig("/repos/lv_port_linux/src/configs/imageview.json")
print(f"loadConfig result:{res}")

lele.addEventHandler("slider0", lambda event: sliderMoved(event))
lele.addEventHandler("push_button0", lambda event: pushButtonPressed(event))

while lele.handleEvents():
    pass



