import os
import sys
import lele

def onRollerValueChanged(language):
   print(f"@@@ onRollerValueChanged: {language}")
   obj = lele.getObjectById("/hello-world-international/label")
   # style = obj.getStyle()
   # print(f"obj style font-size: {style.getValue("font-size")}")
   # print(f"obj style font-family: {style.getValue("font-family")}")
   style = obj.getStyle("style:/hello-world-international/style")
   # print(f"obj style font-size: {style.getValue("font-size")}")
   # print("============================================")
   # user_attributes = [attr for attr in dir(lele) if not attr.startswith('__')]
   # print(f"lele: {user_attributes}")
   # print("============================================")
   # print(f"font_db: {lele.Font().getFontDb()}")
   print("============================================")
   print(f"obj style font-family: {style.getValue("font-family")}")
   if lele.Language().getCurrentLanguage() == "Arabic":
      style.setValue({"font-family":"notosansarabic-bold"})
   else:
      style.setValue({"font-family":"ubuntu-b"})
   print(f"obj style font-family: {style.getValue("font-family")}")
   # print("============================================")
   # print(f"available languages: {lele.Language().getAvailableLanguages()}")
   # print("============================================")
   print(f"current language: {lele.Language().getCurrentLanguage()}")
   lele.Language().setCurrentLanguage(language)
   print("============================================")
   # user_attributes = [attr for attr in dir(style) if not attr.startswith('__')]
   # print(user_attributes)
   # print("============================================")
   return True

res = lele.loadConfig("hello-world-international.json")
if not res:
   sys.exit(1)

label_hello_world = lele.getObjectById("/hello-world-international/label")
print(f"label text: {label_hello_world}")

roller = lele.getObjectById("/hello-world-international/rollerview")
roller.onValueChanged(lambda value: onRollerValueChanged(value))

while lele.handleEvents():
   pass
