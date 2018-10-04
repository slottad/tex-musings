#!/usr/bin/env python

from biggles.libplot.renderer import LibplotRenderer

class DviRenderer(LibplotRenderer):

    def __init__(self, ll, ur, type='X', parameters=None, file=None):
        self.ytrans = ur[1]
        super(DviRenderer, self).__init__(ll, ur, type, parameters, file)

    def flip_y(self, p):
        return (p[0], self.ytrans - p[1])
        
    def rect(self, p, q):
        super(DviRenderer, self).rect(self.flip_y(p), self.flip_y(q))
        
    def text(self, p, s):
        #hstr = self.state.get("texthalign", "center")
        #vstr = self.state.get("textvalign", "center")
        #hnum = super(DviRenderer,self).__pl_text_align.get(hstr)
        #vnum = super(DviRenderer,self).__pl_text_align.get(vstr)
        super(DviRenderer, self).move(self.flip_y(p))
        self.string(ord('l'), ord('b'), s)

        
#width = 850
#height = 1100
bgcolor="white"
ll = 0, 0
ur = 8.5, 11.0
width = int(ur[0] * 100)
height = int(ur[1] * 100)
parameters = {
    "PAGESIZE": "letter,xsize={}in,ysize={}in".format(8.5,11),
    "BITMAPSIZE": "{}x{}".format(width, height),
    "VANISH_ON_DELETE": "no",
    "BG_COLOR":bgcolor
#    "ROTATION": "180"
}
p = DviRenderer(ll,ur,"X",parameters)

p.open()
p.rect((1,10),(7.5,1))
p.set("fontsize", 0.25)
p.set("texthalign", "left")
p.set("textvalign", "bottom")
p.set("fontface", "HersheySerif")

ot2plot = [
    "\*G", "\*D", "\*H", "\*L", "\*C", "\*P", "\*S", "\*U",
    "\*F", "\*Q", "\*W", "ff", "fi", "fl", "ffi", "ffl",
    "\#H0159", "\#H0160", "\#H4181", "\#H4180", "\#H0242", "\#H1250", "\hy", "\de",
    "\ac", "\ss", "\ae", "", "", "\AE", "", "",
    "'", "!", '"', "#", "$", "%", "&", "'",
    "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", ":", ";", "\#H4113", "=", "\#H4114", "?",
    "@", "A", "B", "C", "D", "E", "F", "G",
    "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W",
    "X", "Y", "Z", "[", ",", "]", "^", "\#H4131",
    "`", "a", "b", "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w",
    "x", "y", "z", "\hy", "-", '"', "~", "\#H4182"]

#ot2t = [None] * 128
#for i in range(33,127):
#    ot2t[i] = chr(i)


xorigin = 2.5
yorigin = 2.0
space = 0.5
c = 0
for y in range(0,16):
    for x in range(0,8):
        if ot2plot[c] != None: 
            p.text((xorigin+x*space, yorigin+y*space), ot2plot[c])
            #p.text((xorigin+x*space, yorigin+y*space), "\#H{:04d}".format(ot2t[c]))
        c += 1
        
p.close()

# 0001..0284      alphanumeric symbols, cartographic [small] size
# 0501..0746      alphanumeric symbols, principal [large] size
# 0750..0909      centered symbols
# 0910            alphanumeric symbols, principal [large] size
# 1001..1295      alphanumeric symbols, indexical [medium] size
# 1401..2312      alphanumeric symbols, principal [large] size
# 2317..2382      centered symbols
# 2401..3926      alphanumeric symbols, principal [large] size
# 4000..4194      range contains a large number of such non-Hershey glyphs.
# 4040..4043      Nelson Beebe's large pointing hands
# 4000..4025      range were taken from the UGS glyph repertory.
