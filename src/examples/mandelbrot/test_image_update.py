import os, sys, time, hashlib
import lele

res = lele.loadConfig("mandelbrot.json")
if not res:
    print("FAIL: loadConfig failed")
    sys.exit(1)

# Let it render
for _ in range(10):
    if not lele.handleEvents():
        break

lele.dumpScreenshot()
s1 = "/tmp/screenshot-before.png"
os.rename("/tmp/screenshot-0.png", s1)

# Update with slightly different coords
ok = lele.updateMandelbrotImage("/mandelbrot/image", 1000, 4.0, -1.5, 0.5, -0.75, 0.75)
print(f"updateMandelbrotImage returned: {ok}", flush=True)

for _ in range(30):
    if not lele.handleEvents():
        break

lele.dumpScreenshot()
s2 = "/tmp/screenshot-after.png"
os.rename("/tmp/screenshot-0.png", s2)

# Compare
import hashlib
h1 = hashlib.md5(open(s1,'rb').read()).hexdigest()
h2 = hashlib.md5(open(s2,'rb').read()).hexdigest()
print(f"Screenshots: {s1} (md5={h1}) vs {s2} (md5={h2})", flush=True)
if h1 == h2:
    print("FAIL: Screenshots are identical - image did not update!", flush=True)
    sys.exit(1)
else:
    print("PASS: Screenshots differ - image updated!", flush=True)

sys.exit(0)
