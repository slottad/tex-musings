#!/usr/bin/env python

import biggles.libplot.renderer as r

width = 850
height = 1100
bgcolor="white"
ll = 0.0, 0.0
#ur = width, height
ur = 8.5, 11.0
parameters = {
    "BITMAPSIZE": "%dx%d" % (width, height),
    "VANISH_ON_DELETE": "no",
    "BG_COLOR":bgcolor,
}
p = r.LibplotRenderer(ll,ur,"X",parameters)

p.open()
p.rect((1,10),(7.5,1))
p.set("fontsize", 0.25)
p.set("texthalign", "left")

p.set("fontface", "HersheySerif")
p.set("textvalign", "top")                
p.text((1,10), "This is test text.")

p.set("textvalign", "center")
p.text((1,10), "This is test text.")

p.set("textvalign", "bottom")
p.text((1,10), "This is test text.")

p.set("fontface", "HersheySans")
p.set("textvalign", "top")                
p.text((5,10), "This is test text.")

p.set("textvalign", "center")
p.text((5,10), "This is test text.")

p.set("textvalign", "bottom")
p.text((5,10), "This is test text.")

p.close()
