import sys
print(sys.path)
import time
# from threading import Timer
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
      # style.setValue({'background/rotation/pivot/x':'100'})
      # style.setValue({'background/rotation/pivot/y':'100'})
      style.setValue({'background/rotation/angle':str(theta)})

def setAngleMethod1(theta):
      obj = lele.getObjectById("/view/with_bg_img")
      style = lele.Style(
         '{'
         '"style":{'
         '   "background":{'
         '      "rotation":{"angle":"'+ str(theta) + '","pivot":{"x":"100","y":"100"}}'
         '   }'
         '  }'
         '}')
      obj.addStyle(style)

def scaleImage(event):
   print(f"@@@py SCALE SLIDER MOVED @@@ event.code,lhs:{event.code}, lele.Event().Type.ValueChanged,rhs: {lele.Event().Type.ValueChanged}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.ValueChanged:
         print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("/view/with_bg_img")
         style = obj.getStyle("img")
         dic = style.getValue(["background/size"])
         print(f"@@@py style.dic3 {dic}")
         style.setValue({"background/size":str(event.value)})

         obj = lele.getObjectById("scale-label")
         obj.setText(f"Scale: {event.value}%")
   pass

def tileImage(event):
   print(f"@@@py REPEAT RADIO BUTTON CLIKED @@@ event.code,lhs:{event.code}, lele.Event().Type.Clicked,rhs: {lele.Event().Type.Clicked}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.Clicked:
         print(f"@@@py repeat radio slider {event.object.id}, event.type: {event.code}")
         obj = lele.getObjectById(event.object.id)
         # match event.object.id:
         #    case "repeat-xy":
         #       pass
         #    case "repeat-x":
         #       pass
         #    case "repeat-y":
         #       pass
         #    case "repeat-none":
         #       pass
         #    case _:
         #       return
         obj = lele.getObjectById("/view/with_bg_img")
         style = obj.getStyle("img")
         dic = style.getValue(["background/repeat"])
         print(f"@@@py style.dic4 {dic}")
         style.setValue({"background/repeat":event.object.id})
         
   pass

def rotateImage(event):
   print(f"@@@py ARC MOVED @@@ event.code,lhs:{event.code}, lele.Event().Type.ValueChanged,rhs: {lele.Event().Type.ValueChanged}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.ValueChanged:
         print(f"@@@py arc {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("rotation-label")
         obj.setText(f"Angle: {event.value}°")
         print(f"@@@py change rotation angle by 10 degrees")
         setAngleMethod2(event.value)
   pass

def sliderMoved(event):
   print(f"@@@py SLIDER MOVED @@@ event.code,lhs:{event.code}, lele.Event().Type.ValueChanged,rhs: {lele.Event().Type.ValueChanged}")
   # print(f"@@@py res:{lele.Event().Type} lele.Event().Type.Clicked:{lele.Event().Type.Clicked.value}")
   if event.code == lele.Event().Type.ValueChanged:
         print(f"@@@py slider {event.object.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
         obj = lele.getObjectById("rotation-label")
         obj.setText(f"Angle: {event.value}°")
         print(f"@@@py change rotation angle by 10 degrees")
         setAngleMethod1(event.value)
   pass

def setImgBgColor(rgb):
   obj = lele.getObjectById("/view/with_bg_img")
   print(f"@@@py setImgBgColor obj: {obj}")
   style = obj.getStyle("img")
   print(f"@@@py setImgBgColor style: {style}")
   dic = style.getValue("background/color")
   print(f"@@@py style.dic {dic}")
   dic = style.setValue({'background/color':str(rgb)})
   print(f"@@@py style.dic {dic}")

   #####osm
   obj = lele.getObjectById("rotation-label")
   style = obj.getStyle("views/view0/label")
   dic = style.getValue("height")
   print(f"@@@py views/view0/label dic: {dic}")
   obj = obj.getParent()
   style = obj.getStyle()
   print(f"@@@py views/view0/label parent style: {style}")
   dic = style.getValue("height")
   print(f"@@@py views/view0/label parent dic: {dic}")



def colorChanged(rgb):
   print(f"@@@py colorChanged: {rgb} ({hex(rgb)})")
   setImgBgColor(rgb)
   return

def msgboxEvent(event):
   print(f"@@@py msgboxEvent, button clicked: {event.object.getButtonClicked().id}, button text: {event.object.getButtonClicked().getText()}")
   return True

_tests_are_running = False
_scale_delta = 1
_rotate_delta = 1

def runTest(event):
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
      # obj = lele.getObjectById("rotation-label")
      # print(f"@@@py getObjectById {obj}, obj.getText() {obj.getText()}")
      # obj.setText("Cocoa bean!")

      # print(f"@@@py change text color to white")
      # style = lele.Style('{"style":{"fgcolor":"#fff"}}')
      # obj.addStyle(style)

      # print(f"@@@py======================================")
      # print(f"@@@py type(lele.MessageBox) {type(lele.MessageBox)}")
      # root = lele.getObjectById("/views:0/view")
      # msgbox = root.addChild("../messagebox/messagebox.json")
      # msgbox.addEventHandler(lambda event: msgboxEvent(event))

      # msgbox = lele.MessageBox(root, "/repos/lv_port_linux/src/examples/messagebox/messagebox.json")
      #osm todo: test above msgbox can do handle events
      #osm: now we have above 2 ways of loading an object from its config!

      # obj = lele.getObjectById("scale")
      # for i in range(100):
      #    time.sleep(0.01) 
      #    obj.setValue(i)
      global _tests_are_running
      _tests_are_running ^= True
      #osm todo: change image to stop button, and change label also
      obj = lele.getObjectById("img:/run_test")
      user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
      print(f"@@@py obj: {obj}, user_attributes: {user_attributes}.")
      obj = lele.getObjectById("label:/run_test")
      img = lele.getObjectById("img:/run_test")
      if _tests_are_running:
          obj.setText("Stop")
          img_src = img.getSrc()
          print(f"@@@py img_src: {img_src}.")
          img.setSrc("res/pause-button.png")
      else:
          obj.setText("Run Test")
          img.setSrc("res/play-button.png")
      user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
      print(f"@@@py obj: {obj}, user_attributes: {user_attributes}.")
      # lele.getObjectById("repeat-none").click()
      pass

def runTestLoop():
   global _tests_are_running
   global _scale_delta
   global _rotate_delta
   if _tests_are_running:
      obj = lele.getObjectById("scale")
      scale = obj.getValue() % 200
      if scale >= 200-1:
         _scale_delta = -1
      elif scale == 1:
         _scale_delta = 1
      obj.setValue(scale + _scale_delta)
      obj = lele.getObjectById("rotate")
      angle = obj.getValue()
      if angle == 360 :
         _rotate_delta = -1
      elif angle == 0:
         _rotate_delta = 1
      obj.setValue(angle + _rotate_delta)
      if scale > 0 and scale <= 50:
         lele.getObjectById("repeat-none").click()
      elif scale > 50 and scale <= 100:
         lele.getObjectById("repeat-x").click()
      elif scale > 100 and scale <= 150:
         lele.getObjectById("repeat-y").click()
      elif scale > 150 and scale <= 200:
         lele.getObjectById("repeat").click()
      time.sleep(0.005) 
   pass

def main():
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
   # lele.getObjectById("scale").addEventHandler(lambda event: scaleImage(event))
   lele.addEventHandler("scale", lambda event: scaleImage(event))
   lele.addEventHandler("rotate", lambda event: rotateImage(event))
   lele.addEventHandler("repeat", lambda event: tileImage(event))
   lele.addEventHandler("repeat-x", lambda event: tileImage(event))
   lele.addEventHandler("repeat-y", lambda event: tileImage(event))
   lele.addEventHandler("repeat-none", lambda event: tileImage(event))
   lele.addEventHandler("run_test", lambda event: runTest(event))

   btn = lele.Button()
   print(f"@@@py btn: {btn}, attr: {[attr for attr in dir(btn.Type) if not attr.startswith('__')]}")
   # for attr in btn.Type:
   #    print(f"@@@py attr.name {attr.name}, attr.value: {attr.value}")
   obj = lele.getObjectById("rotation-label")
   if obj:
      user_attributes = [attr for attr in dir(obj) if not attr.startswith('__')]
   obj = lele.getObjectById("run_test")
   if obj:
      user_attributes = [attr for attr in dir(obj.Type) if not attr.startswith('__')]
      print(f"@@@py run_test.Type {obj.Type}, user_attributes: {user_attributes}")
      # for attr in obj.Type:
      #    print(f"@@@py attr.name {attr.name}, attr.value: {attr.value}")

   obj = lele.getObjectById("/view/with_bg_img")
   style = obj.getStyle()
   print(f"@@@py======================================1")
   print(f"@@@py lele.Style().Border: {lele.Style().Border}")
   print(f"@@@py style:{style}")
   print(f"@@@py======================================")

   obj = lele.getObjectById("rotation-label")
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

   colorwheel = lele.getObjectById("colorwheel")
   rgb = colorwheel.getColor()
   print(f"@@@py colorwheel.getColor:{hex(rgb)}")
   colorwheel.onColorChanged(lambda rgb: colorChanged(rgb))

   while lele.handleEvents():
      runTestLoop()
      pass

try:
    main()
except:
   print(traceback.format_exc())
