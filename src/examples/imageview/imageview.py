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

def setAngleMethod2(theta):
      print(f"@@@py======================================1")
      obj = lele.getObjectById("/view/with_bg_img")
      style = obj.getStyle("img")
      # print(f"@@@py style {style}")
      # print(f"@@@py style.className() {style.getClassName()}")
      # print(f"@@@py style.getValue() {style.getValue()}")
      dic = obj.getStyle()
      print(f"@@@py style.dic0  {dic}")
      dic = style.getValue(["background/image", "background/repeat", "background/rotation/angle", "background/rotation/pivot", "background/rotation/pivot/x", "background/rotation/pivot/y"])
      angle = dic['background/rotation/angle']
      print(f"@@@py style.dic1 {dic}, angle {angle}")
      # print(f"@@@py set background/repeat ")
      # style.setValue({'background/repeat':'no'})
      # print(f"@@@py get background/repeat")
      # dic = style.getValue(['background/repeat'])
      print(f"@ @@py style.dic2 {dic}")
      # style.setValue({'background/rotation/pivot/x':'256'})
      # style.setValue({'background/rotation/pivot/y':'271'})
      style.setValue({'background/rotation/angle':str(theta)})

def setAngleMethod1(theta):
      obj = lele.getObjectById("/view/with_bg_img")
      style = lele.Style(
         '{'
         '"style":{'
         '   "background":{'
         '      "rotation":{"angle":"'+ str(theta) + '","pivot":{"x":"256","y":"271"}}'
         '   }'
         '  }'
         '}')
      obj.addStyle(style)


def arcMoved(event):
   print(f"@@@py ARC MOVED @@@ event.code,lhs:{event.code}, lele.Event().Type.ValueChanged,rhs: {lele.Event().Type.ValueChanged}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.ValueChanged:
         print(f"@@@py arc {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("/views:0/view/label")
         obj.setText(f"Angle: {event.value}°")
         print(f"@@@py change rotation angle by 10 degrees")
         setAngleMethod2(event.value)
   pass

def sliderMoved(event):
   print(f"@@@py SLIDER MOVED @@@ event.code,lhs:{event.code}, lele.Event().Type.ValueChanged,rhs: {lele.Event().Type.ValueChanged}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.ValueChanged:
         print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("/views:0/view/label")
         obj.setText(f"Angle: {event.value}°")
         print(f"@@@py change rotation angle by 10 degrees")
         setAngleMethod1(event.value)
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
   # print(f"@@@py:35 res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked}")
   if event.code == lele.Event().Type.Clicked:
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

try:
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
   lele.addEventHandler("arc0", lambda event: arcMoved(event))
   lele.addEventHandler("push_button0", lambda event: pushButtonPressed(event))

   btn = lele.Button()
   print(f"@@@py btn: {btn}, attr: {[attr for attr in dir(btn.Type) if not attr.startswith('__')]}")
   # for attr in btn.Type:
   #    print(f"@@@py attr.name {attr.name}, attr.value: {attr.value}")
   obj = lele.getObjectById("/views:0/view/label")
   if obj:
      user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
   obj = lele.getObjectById("push_button0")
   if obj:
      user_attributes = [attr for attr in dir(obj.Type) if not attr.startswith('__')]
      print(f"@@@py push_button0.Type {obj.Type}, user_attributes: {user_attributes}")
      # for attr in obj.Type:
      #    print(f"@@@py attr.name {attr.name}, attr.value: {attr.value}")

   obj = lele.getObjectById("/view/with_bg_img")
   style = obj.getStyle()
   print(f"@@@py======================================1")
   print(f"@@@py lele.Style().Border: {lele.Style().Border}")
   print(f"@@@py style:{style}")
   print(f"@@@py======================================")

   obj = lele.getObjectById("/views:0/view/label")
   # print(f"@@@py getObjectById {obj}, obj.getText() {obj.getText()}")
   obj.setText("Cocoa bean!")

   print(f"@@@py======================================x")
   print(f"event.Type: {lele.Event().Type}")
   user_attributes = [attr for attr in dir(lele.Event().Type) if not attr.startswith('__')]
   print(f"event.Type: {user_attributes}")
   print(f"event.Type.Clicked: {lele.Event().Type.Clicked}")
   # import enum
   # lele.FooBar = enum.Enum('FooBar', dict(FOO=1, BAR=2))
   # user_attributes = [attr for attr in dir(lele.FooBar) if not attr.startswith('__')]
   # print(f"lele.FooBar: {user_attributes}")


   print(f"@@@py======================================")
   user_attributes = [attr for attr in dir(lele.Event) if not attr.startswith('__')]
   print(f"lele.Event: {user_attributes}")
   event = lele.Event()
   print(f"event: {event}")

   print(f"@@@py======================================")
   user_attributes = [attr for attr in dir(lele.Style()) if not attr.startswith('__')]
   print(f"lele.Style(): {user_attributes}")
   style = lele.Style('{"style":{"fgcolor":"#fff"}}')
   obj.addStyle(style)

   print(f"@@@py======================================")
   user_attributes = [attr for attr in dir(style.Layout) if not attr.startswith('__')]
   print(f"@@@py style.Layout: {style.Layout}, user_attributes: {user_attributes}.")
   user_attributes = [attr for attr in dir(style.Flow) if not attr.startswith('__')]
   print(f"@@@py style.Flow: {style.Flow}, user_attributes: {user_attributes}.")
   user_attributes = [attr for attr in dir(style.Scrollbar) if not attr.startswith('__')]
   print(f"@@@py style.Scrollbar: {style.Scrollbar}, user_attributes: {user_attributes}.")
   user_attributes = [attr for attr in dir(style.Border) if not attr.startswith('__')]
   print(f"@@@py style.Border: {style.Border}, user_attributes: {user_attributes}.")

   while lele.handleEvents():
      pass
except:
   print(traceback.format_exc())
