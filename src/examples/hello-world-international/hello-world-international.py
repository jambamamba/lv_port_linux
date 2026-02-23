import os
import sys
import lele

def setFont(current_lang):
   obj = lele.getObjectById("/hello-world-international/label")
   style = obj.getStyle("style:/hello-world-international/style")
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

def onRollerValueChanged(language):
   lele.Language().setCurrentLanguage(language)
   setFont(lele.Language().getCurrentLanguage())
   return True

res = lele.loadConfig("hello-world-international.json")
if not res:
   sys.exit(1)

label_hello_world = lele.getObjectById("/hello-world-international/label")
print(f"label text: {label_hello_world}")

roller = lele.getObjectById("/hello-world-international/rollerview")
roller.onValueChanged(lambda value: onRollerValueChanged(value))

# print(f"selected item: {roller.getSelectedItem()}")
# roller.setSelectedItem("Arabic")
# print(f"selected item index: {roller.getSelectedItemIndex()}")
# roller.setSelectedItemIndex(2)

while lele.handleEvents():
   pass
