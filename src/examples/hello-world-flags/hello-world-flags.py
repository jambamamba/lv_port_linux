import os
import sys
import lele

def setFont(current_lang):
   obj = lele.getObjectById("/hello-world-international/label")
   style = obj.getStyleById("style:/hello-world-international/style")
   # print(f"obj style font-size: {style.getValue("font-size")}")
   # print("============================================")
   if (current_lang == "Arabic" or 
       current_lang == "Persian"):
      style.setValue({"font-family":"NotoSansArabic-Bold"})
   elif (current_lang == "Hebrew"):
      style.setValue({"font-family":"NotoSansHebrew-Black"})
   elif (current_lang == "Hindi"):
      style.setValue({"font-family":"NotoSerifDevanagari-Bold"})
   elif (current_lang == "Thai"):
      style.setValue({"font-family":"NotoSansThai-Medium"})
   elif (current_lang == "Chinese" or
         current_lang == "Japanese" or
         current_lang == "Vietnamese" or
         current_lang == "Burmese"):
      style.setValue({"font-family":"NotoSansSC-VariableFont_wght"})
   else:
      style.setValue({"font-family":"Ubuntu-B"})
   print(f"current_lang: {current_lang}")
   print(f"obj style font-family: {style.getValue("font-family")}")
   print("============================================")

def sliderEventHandler(event):
   if (event.code != lele.Event().Type.EVENT_SCROLL):
      return
   slider = lele.getObjectById("/slider")
   posx = event.target.getScrollX()
   posx = slider.getScrollX()
   views = slider.getChildren()
   for view in views:
      # print(f"@@@py view : {view.id}")
      view.removeClass("pane/selected")
      img = view.getChildById(f"{view.id}/img")
      if img:
         img.removeClass("pane/selected/img")
         # width = view.getStyleAttribute("width") #osm todo: calling getStyleAttribute crashes
         # print(f"@@@py width: {width}")
   # print(f"@@@py sliderEventHandler id:{event.id}, code:{event.code}, value:{event.value}, scroll_x:{event.target.getScrollX()}")
   # when first element is centered: -384
   # each element's width: 267
   center_element = (event.target.getScrollX() - (-384))/267
   center_element = int(center_element)
   # width = center_element.getStyleAttribute("width")
   # print(f"@@@py width: {width}")
   # print(f"@@@py center_element: {center_element}")
   view = lele.getObjectById("/pane" + str(center_element))
   if view:
      img = view.getChildById(f"{view.id}/img")
      if img:
         img.addClass("pane/selected/img")
         language = view.getAttribute("language")
         lele.Language().setCurrentLanguage(language)
         setFont(lele.Language().getCurrentLanguage())

      view.addClass("pane/selected")

   # user_attributes = [attr for attr in dir(event) if not attr.startswith('__')]
   # print(user_attributes)
   # if (event.code == lele.Event().Type.EVENT_SCROLL_END or event.code == lele.Event().Type.EVENT_SCROLL):
      # print(f"@@@ event.code:{event.code}, event.id:{event.id}")
   #       print(f"@@@py slider {event.target.id} moved to: {event.value}%, event.type: {event.code}, event.action: {event.action}")
   #       obj = lele.getObjectById("/view/with_bg_img")
   #       style = obj.getStyleById("img")
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
