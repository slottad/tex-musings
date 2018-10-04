#!/usr/bin/env python
import argparse
import math
import struct
import sys
import copy

import texfonts

def signed(b, arr):
    v = unsigned(b,arr)
    bits = b*8;
    mask = 2**(bits - 1)
    return -(v & mask) + (v & ~mask)

def unsigned(b, arr):
    t = 0
    for i in range(b):
        t *= 256
        t += arr[i]
    return t

class Coordinates:
    def __init__(self):
        self.reset()

    def reset(self):
        self.h = 0
        self.v = 0
        self.w = 0
        self.x = 0
        self.y = 0
        self.z = 0

    def __str__(self):
        return "(h={},v={},w={},x={},y={},z={})".format(self.h,
                                                        self.v,
                                                        self.w,
                                                        self.x,
                                                        self.y,
                                                        self.z)

# scale = mag * num / 1000 * den
# dimension * scale = 10^-7 meters
# There are 2.54cm in a inch
def dvi2in(scale, dim):
    return dim * scale * 10**(-7) / 0.0254

def in2pt(inch):
    return inch*72.27

def m2in(m):
    return m/0.0254

def in2m(m):
    return m*0.0254

def pt2in(pt):
    return pt/72.27

class DviReader:

    def __init__(self, dvifile, dpi=300.0):
        # DVI files are so tiny and cute!
        self.fb = dvifile.read()
        self.dpi = dpi
        self.ver = None
        self.num = None
        self.fonts = {}
        self.setup_handlers()
        #self.parse_post()
        #self.parse_header()
        self.loc = 0
        self.stack = []
        self.coords = Coordinates()
        while self.next_op() != 248:
            pass
        #for i in range(100):

        #if self.getop() == 139:
        #    self.beginning_of_page()

    def next_op(self):
        op = self.getop()
        f = self.handlers[op]
        f(op)
        return op
    
    def set_ver(self,v):
        if self.ver == None:
            self.ver = v
            return
        if self.ver != v:
            print("Version mismatch, {} != {}".format(self.v, v))
            sys.exit(-1)

    def set_num(self,n):
        if self.num == None:
            self.num = n
            return
        if self.num != n:
            print("Dimension mismatch, {} != {}".format(self.n, n))
            sys.exit(-1)

    def setup_handlers(self):
        self.handlers = 256*[None]
        for i in range(0,128):
            self.handlers[i] = self.set_char_i
        for i in range(128,132):
            self.handlers[i] = self.seti
        self.handlers[132] = self.set_rule
        for i in range(133,137):
            self.handlers[i] = self.puti
        self.handlers[137] = self.put_rule
        self.handlers[138] = self.nop
        self.handlers[139] = self.bop
        self.handlers[140] = self.eop
        self.handlers[141] = self.push
        self.handlers[142] = self.pop
        for i in range(143,147):
            self.handlers[i] = self.righti
        for i in range(147,152):
            self.handlers[i] = self.wi
        for i in range(152,157):
            self.handlers[i] = self.xi
        for i in range(157,161):
            self.handlers[i] = self.downi
        for i in range(161,166):
            self.handlers[i] = self.yi
        for i in range(166,171):
            self.handlers[i] = self.zi
        for i in range(171,235):
            self.handlers[i] = self.fnt_num_i
        for i in range(235,239):
            self.handlers[i] = self.fnti
        for i in range(239,243):
            self.handlers[i] = self.special
        for i in range(243,247):
            self.handlers[i] = self.fnt_defi
        self.handlers[247] = self.pre
        self.handlers[248] = self.post

    # Opcodes 0-127
    def set_char_i(self, op):
        c = self.cur_font.basic_char(op)
        self.coords.h += c.height
        print("set_char_{}: {}".format(op, self.cur_font.basic_char(op)))

    # Opcodes 128-131
    def seti(self, op):
        i = abs(127-op)
        k = self.getuint(i)
        c = self.cur_font.basic_char(k)
        self.coords.h += c.height
        print("set{}: {}, {}".format(i,k, self.cur_font.basic_char(k)))

    # Opcode 132
    def set_rule(self, op):
        fmt = '>ii'
        (a,b) = struct.unpack_from(fmt, self.fb[self.loc])
        self.loc += struct.calcsize(fmt)
        self.coords.h += b
        print("rule a:{}, b:{}".format(a,b))

    # Opcodes 133-136
    def puti(self, op):
        i = abs(132-op)
        k = self.getuint(i)
        print("put{}: {}".format(i,k))

    # Opcode 137
    def put_rule(self, op):
        fmt = '>ii'
        (a,b) = struct.unpack_from(fmt, self.fb[self.loc])
        self.loc += struct.calcsize(fmt)
        print("put_rule")
        
    # Opcodes 138
    def nop(self, op):
        print('nop')

    # Opcode 139
    def bop(self, op):
        fmt = '>11i'
        counts = struct.unpack_from(fmt, self.fb, self.loc)
        self.loc += struct.calcsize(fmt)
        prevpage=counts[10]
        counts = counts[0:10]
        self.stack = []
        self.cur_font = None
        self.coords = Coordinates()
        print("bop")

    # Opcodes 140
    def eop(self, op):
        print('eop')

    # Opcodes 141
    def push(self, op):
        self.stack.append(copy.copy(self.coords))
        print('push')

    # Opcodes 142
    def pop(self, op):
        self.coords = self.stack.pop()
        print('pop')

    # Opcodes 143-146
    def righti(self, op):
        i = abs(142-op)
        b = self.getint(i)
        self.coords.h += b
        print("right{}: {}".format(i, b))
        
    # Opcodes 147-151
    def wi(self, op):
        i = abs(147-op)
        b = 0
        if i > 0:
            b = self.getint(i)
            self.coords.w = b
            self.coords.h += b
        else:
            self.coords.h += self.coords.w
        print("w{}: {}\t{}".format(i, b, self.coords))
        
    # Opcodes 152-156
    def xi(self, op):
        i = abs(152-op)
        b=0
        if i >  0:
            b = self.getint(i)
            self.coords.x = b
            self.coords.h += b
        else:
            self.coords.h += self.coords.x
        print("x{}: {}".format(i, b))       
        
    # Opcodes 157-160
    def downi(self, op):
        i = abs(156-op)
        a = self.getint(i)
        self.coords.v += a
        print("down{}: {}".format(i, a))
        
    # Opcodes 161-165
    def yi(self, op):
        i = abs(161-op)
        a=0
        if i > 0:
            a = self.getint(i)
            self.coords.y = a
            self.coords.v += a
        else:
            self.coords.v += self.coords.y
        print("y{}: {}".format(i, a))
        
    # Opcodes 166-170
    def zi(self, op):
        i = abs(166-op)
        a = 0
        if i > 0:
            a = self.getint(i)
            self.coords.z = a
            self.coords.v += a
        else:
            self.coords.v += self.coords.z
        print("z{}: {}".format(i, a))
        
    # Opcodes 171-234
    def fnt_num_i(self, op):
        i = abs(171-op)
        self.cur_font = self.fonts[i]
        print("fnt_num_{}, {}".format(i,self.fonts[i]))

    # Opcodes 235-238
    def fnti(self, op):
        i = abs(234-op)
        k = self.getuint(i)
        self.cur_font = self.fonts[k]
        print("fnt{}: {}, {}".format(i,k,self.fonts[k]))

    # Opcodes 239-242
    def special(self, op):
        i = abs(238-op)
        k = self.getuint(i)
        print("special{}: {}".format(i, k))
        self.loc += k+1

    # Opcodes 243-246
    def fnt_defi(self, op):
        i = abs(242-op)
        k = self.getuint(i)
        fmt='IIIBB'
        (c,s,d,a,l) = struct.unpack_from(fmt, self.fb, self.loc)
        self.loc += struct.calcsize(fmt)
        fontdir = ''
        if a > 0:
            fontdir = str(self.fb[self.loc:self.loc+a], encoding='ascii')
            self.loc += a
        fontname = str(self.fb[self.loc:self.loc+l], encoding='ascii')
        self.loc += l
        print("fnt_def{}: {}, name: {}, dir: {}".format(i, k, fontname, fontdir))
        self.fonts[k] = texfonts.dvifont(k,c,s,d,fontdir,fontname)

    # Opcode 247
    def pre(self, op):
        fmt = '>BIIIB'
        (ver, num, self.den, self.mag, k) = struct.unpack_from(fmt, self.fb, self.loc)
        self.set_ver(ver)
        self.set_num(num)
        self.scale = (self.mag*self.num)/float(1000.0*self.den)
        offset = struct.calcsize(fmt)
        end = offset+k
        self.comment = str(self.fb[offset:end], encoding='ascii')
        self.loc = end+1
        print("pre")

    def find_post(self):
        loc = len(self.fb)-1
        while self.fb[loc] == 223:
             loc += -1
        loc += -4
        fmt = '>IB'
        (loc,ver) = struct.unpack_from(fmt, self.fb, loc)
        self.set_ver(ver)
        self.post_loc = loc

    # Opcode 248
    def post(self, op):
        fmt = '>IIIIIIHH'
        (last_page, num, den, mag,
         self.max_height, self.max_width,
         self.max_stack, self.max_pages) = struct.unpack_from(fmt, self.fb, self.loc)
        self.set_num(num)
        self.loc += struct.calcsize(fmt)
        print("post")
        # op = self.getop()
        # while op != 249:
        #     f = self.handlers[op]
        #     #fnt = self.font_defi(i)
        #     #self.fonts[fnt.id] = fnt
        #     op = self.getop()

    def getop(self):
        op = struct.unpack_from('B', self.fb, self.loc)[0]
        print("loc: {}, op: {}, ".format(self.loc, op),end='')
        self.loc += 1
        return op

    def getint(self,n):
        l = self.loc
        v = signed(n, self.fb[l:l+n])
        self.loc += n
        return v

    def getuint(self,n):
        l = self.loc
        v = unsigned(n, self.fb[l:l+n])
        self.loc += n
        return v


    # def unit2inch(self, u):
    #     return m2in(u*self.scale*10**(-7))

    def print_info(self):
        print("This is {}, Version {}, (VectorTex {})".format(__file__,0.01,2018))
        print("Options selected:")
        print("  Resolution = {} pixels per inch".format(self.dpi))
        print("mag*num/1000*den = scale")
        print("({}*{})/(1000*{}) = {}".format(self.mag, self.num, self.den, self.scale))
        print("'{}'".format(self.comment))
        print("Postamble starts at byte {} out of {} total bytes".format(self.post_loc,
                                                                     len(self.fb)))
        print("maxv={}, maxh={}, maxstackdepth={}, totalpages={}".format(self.max_height,
                                                                         self.max_width,
                                                                         self.max_stack,
                                                                         self.max_pages))
        h = dvi2in(self.scale, self.max_height)
        w = dvi2in(self.scale, self.max_width)
        print("Height={:.2f}in, Width={:.2f}in".format(h, w))
        hpt = in2pt(h)
        wpt = in2pt(w)
        print("Height={:.2f}pt, Width={:.2f}pt".format(hpt, wpt))
        for f in self.fonts.values():
            print(f)



if __name__=='__main__':
    if sys.version_info[0] < 3:
        raise "Must be using Python 3"
    parser = argparse.ArgumentParser(
        description='Plot dvi file using libplot.')
    parser.add_argument('-T', '--format',
                       choices=['X', 'png', 'meta', 'gif', 'svg', 'ps', 'fig', 'hpgl'],
                       default='X',
                       help='The output format (default: %(default)s).')
    parser.add_argument('dvifile', type=argparse.FileType('rb'))
    args = parser.parse_args()
    #print(args.dvifile)
    dvi = DviReader(args.dvifile)
    #dvi.print_info()
