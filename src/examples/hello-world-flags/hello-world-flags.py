import os
import sys
import lele

def sliderEventHandler(event):
   if (event.code != lele.Event().Type.EVENT_SCROLL):
      return
   slider = lele.getObjectById("/slider")
   posx = event.target.getScrollX()
   posx = slider.getScrollX()
   deltax = 418-151
   # style = slider.getStyle()
   # if style:
   #    print(f"@@@py slider style.Keys: {style.Keys}")
   views = slider.getChildren()
   for view in views:
      # print(f"@@@py view : {view.id}")
      view.removeClass("pane/selected")
      img = view.getChildById(f"{view.id}/img")
      if img:
         img.removeClass("pane/selected/img")
   # print(f"@@@py sliderEventHandler id:{event.id}, code:{event.code}, value:{event.value}, scroll_x:{event.target.getScrollX()}")
   # when first element is centered: -384
   # each element's width: 267
   center_element = (event.target.getScrollX() - (-384))/267
   center_element = int(center_element)
   # print(f"@@@py center_element: {center_element}")
   view = lele.getObjectById("/pane" + str(center_element))
   if view:
      img = view.getChildById(f"{view.id}/img")
      if img:
         img.addClass("pane/selected/img")
      view.addClass("pane/selected")

   # user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
   # print(user_attributes)
   # if (event.code == lele.Event().Type.EVENT_SCROLL_END or event.code == lele.Event().Type.EVENT_SCROLL):
      # print(f"@@@ event.code:{event.code}, event.id:{event.id}")
   #       print(f"@@@py slider {event.target.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
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

# lele.getObjectById("/container").addEventHandler(lambda event: sliderEventHandler(event))
lele.getObjectById("/slider").addEventHandler(lambda event: sliderEventHandler(event))
# lele.addEventHandler("/slider", lambda event: sliderEventHandler(event))
# objs = lele.getObjectById("/pane0")
# print(f"@@@ objs len: {len(objs)}")
while lele.handleEvents():
   pass
