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
   # print(f"@@@py res:{lele.Event.Type} lele.Event.Type.Clicked:{lele.Event.Type.Clicked.value}")
   if event.code == lele.Event.Type.ValueChanged:
         print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("/views:0/view/label")
         obj.setText(f"Angle: {event.value}°")
         print(f"@@@py change rotation angle by 10 degrees")
         obj = lele.getObjectById("/view/with_bg_img")
         style = lele.Style(
            '{'
            '"style":{'
            '   "background":{'
            '      "rotation":{"angle":"' + str(event.value) + '","pivot":{"x":"256","y":"271"}}'
            '   }'
            '  }'
            '}')
         obj.addStyle(style)
   pass

def msgboxEvent(event):
   print(f"@@@py msgboxEvent, button clicked: {event.object.getButtonClicked().id}, button text: {event.object.getButtonClicked().getText()}")
   return True

def pushButtonPressed(event):
   # print(f"@@@py event: {event}")
   # user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
   # print(user_attributes)
   # print(f"@@@py event.object: {event.object}")
   # user_attributes = [attr for attr in dir(event.object) if not attr.startswith('__')]
   # print(user_attributes)
   # for event_type in event.Type:
   #    print(f"@@@py enum event.Type: Name: {event_type.name}, Value: {event_type.value}")
   # for name, value in globals().items():
   #    if not name.startswith('__'):
   #       print(f"{name}: {value}")
   # print("...")
   # user_attributes = [attr for attr in dir(lele.Button) if not attr.startswith('__')]
   # print(f"@@@py:33 lele.Button attributes: {user_attributes}")
   # print(f"@@@py:34 res:{lele.Button.Type} lele.Button.Type.Push:{lele.Button.Type.Push.value}")
   # print(f"@@@py:35 res:{lele.Event.Type} lele.Event.Type.Clicked:{lele.Event.Type.Clicked}")
   if event.code == lele.Event.Type.Clicked:
      # print(f"@@@py: event: {event}")
      # user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
      # print(user_attributes)
      # print(f"@@@py: event.Type.Clicked: {event.Type.Clicked}")
      # user_attributes = [attr for attr in dir(event.Type.Clicked) if not attr.startswith('__')]
      # print(user_attributes)
      # print(f"@@@py button pressed, event.object: {event.object}")
      # user_attributes = [attr for attr in dir(event.object) if not attr.startswith('__')]
      # print(user_attributes)
      # print(f"@@@py event.object.getText(): {event.object.getText()}")
      # print(f"@@@py event.object: {event.object.id}")
      obj = lele.getObjectById("/views:0/view/label")
      # print(f"@@@py getObjectById {obj}, obj.getText() {obj.getText()}")
      obj.setText("Cocoa bean!")

      print(f"@@@py change text color to white")
      style = lele.Style('{"style":{"fgcolor":"#fff"}}')
      obj.addStyle(style)

      print(f"@@@py======================================")
      print(f"@@@py type(lele.MessageBox) {type(lele.MessageBox)}")
      root = lele.getObjectById("/views:0/view")
      msgbox = root.addChild("../messagebox/messagebox.json")
      msgbox.addEventHandler(lambda event: msgboxEvent(event))

      # msgbox = lele.MessageBox(root, "/repos/lv_port_linux/src/examples/messagebox/messagebox.json")
      #osm todo: test above msgbox can do handle events
      #osm: now we have above 2 ways of loading an object from its config!
   pass

res = lele.foo(
        10,
        "hello",
        ["/home", "/tmp"], 
        {"flags":"bar", "sue": "sal"},
        lambda percent: callback(percent))
print(f"@@@py foo result:{res}")

# res = lele.loadConfig("/repos/lv_port_linux/src/examples/imageview/imageview.json")
res = lele.loadConfig("imageview.json")
if not res:
   print(f"@@@py Failed to load config. loadConfig result:{res}")
print(f"@@@py loadConfig result:{res}")

lele.addEventHandler("slider0", lambda event: sliderMoved(event))
lele.addEventHandler("push_button0", lambda event: pushButtonPressed(event))

obj = lele.getObjectById("/views:0/view/label")
if obj:
   user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
obj = lele.getObjectById("push_button0")
if obj:
   for button_type in obj.Type:
      print(f"@@@py enum button.Type: Name: {button_type.name}, Value: {button_type.value}")


try:
   while lele.handleEvents():
      pass
except:
   print(traceback.format_exc())
