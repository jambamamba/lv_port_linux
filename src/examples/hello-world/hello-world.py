import os
import sys
import lele

res = lele.loadConfig("hello-world.json")
if not res:
   sys.exit(1)

label_hello_world = lele.getObjectById("/hello-world/label")
print(f"label text: {label_hello_world}")

label_version = lele.getObjectById("/hello-world/version")
version = f"{lele.version()['Major']}.{lele.version()['Minor']}"
label_version.setText(f"Lele-UI Version: {version}")

print("============================================")
user_attributes = [attr for attr in dir(lele) if not attr.startswith('__')]
print(user_attributes)
print("============================================")
lele_font = lele.Font()
font_db = lele_font.getFontDb()
user_attributes = [attr for attr in dir(font_db) if not attr.startswith('__')]
print(user_attributes)
print(f"font_db: {font_db}")
print("============================================")

while lele.handleEvents():
   pass
