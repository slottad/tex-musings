#!/usr/bin/env python
from collections import namedtuple
import struct
import subprocess

Counts = namedtuple('Counts',
                    ('lf', 'lh', 'bc', 'ec', 'nw', 'nh',
                     'nd', 'ni', 'nl', 'nk', 'ne', 'np'))

#locat = namedtuple('Header', ()

Char = namedtuple('Char',
                  ('width', 'height', 'depth',
                   'italic', 'tag', 'remainder'))
BasicChar = namedtuple('BasicChar', ('width', 'height', 'depth'))

class Locations:
    """docstring for Locations"""
    def __init__(self, counts):
        self.header = 24
        self.finfo = self.header + (counts.lh * 4)
        self.widths = self.finfo + ((counts.ec - counts.bc + 1) * 4)
        self.heights = self.widths + (counts.nw * 4)
        self.depths = self.heights + (counts.nh * 4)
        self.charic = self.depths + (counts.nd * 4)
        
    def __str__(self):
        return "finfo: {}\nwidths: {}\nheights: {}\ndepths: {}\n".format(
            self.finfo, self.widths, self.heights, self.depths)

FIXSCALE = 2**-20
def fix2float(fix):
    return FIXSCALE*fix

class TFMFont:
    """docstring for TFMFont"""
    
    def __init__(self, name):
        self.name = name

        self.read()

    def kpse(self):
        self.path = ""
        filename = self.name + ".tfm"
        r = subprocess.run(['kpsewhich', filename],
                           stdout=subprocess.PIPE)
        if r.returncode == 0:
            self.path = str(r.stdout,encoding='ascii').strip()

    def read(self):
        self.kpse()
        if len(self.path) == 0:
            return

        # Get all bytes
        fb = open(self.path, 'rb').read()

        # Read counts
        fmt = '>12H'
        counts = Counts._make(struct.unpack_from(fmt, fb, 0))
        #print(counts)
        loc = Locations(counts)
        #print(loc)
        
        # Get nominal size
        fmt = '>LL'
        (self.checksum, ds) = struct.unpack_from(fmt, fb, loc.header)
        self.design_size = fix2float(ds)

        # Read widths
        fmt = '>{}L'.format(counts.nw)
        self.widths = list(map(fix2float,struct.unpack_from(fmt, fb, loc.widths)))
        #print(self.widths)

        # Read heights
        fmt = '>{}L'.format(counts.nh)
        self.heights = list(map(fix2float,struct.unpack_from(fmt, fb, loc.heights)))
        #print(self.heights)

        # Read depths
        fmt = '>{}L'.format(counts.nd)
        self.depths = list(map(fix2float,struct.unpack_from(fmt, fb, loc.depths)))
        #print(self.depths)

        # Read charic
        fmt = '>{}L'.format(counts.ni)
        self.charic = list(map(fix2float,struct.unpack_from(fmt, fb, loc.charic)))
        #print(self.charic)


        # Read finfo array
        fmt = '>BBBB'
        sz = struct.calcsize(fmt)
        self.finfo = []
        self.bfinfo = []
        for p in range(loc.finfo,loc.widths,sz):
            #print(p)
            (wI, hdI, ct, rem) = struct.unpack_from(fmt, fb, p)
            #print(wI, hdI, ct, rem)
            #print(wI, int(hdI >> 4), int(hdI & 0x0F), int(ct >> 2), ct & 0x03, rem)
            char = Char._make((
                self.widths[wI]*self.design_size,
                self.heights[int(hdI >> 4)]*self.design_size,
                self.depths[int(hdI & 0x0F)]*self.design_size,
                self.charic[int(ct >> 2)]*self.design_size,
                ct & 0x03,
                rem))
            self.finfo.append(char)
            bchar = BasicChar._make((char.width,char.height,char.depth))
            self.bfinfo.append(bchar)
            #print(char)


    def __str__(self):
        s = """
        Path: {}
        Checksum: {:x}
        Design Size: {}
        """.format(self.path, self.checksum, self.design_size)
        return s

class dvifont:
    def __init__(self,k,c,s,d,fd,fn):
        self.id = k
        self.checksum = c
        self.scale = s
        self.design_size = d
        self.dir = fd
        self.name = fn
        self.font = TFMFont(self.name)

    def __str__(self):
        return "Font {}: {}, scale {}. Checksum: {:x}".format(self.id,
                                                              self.name,
                                                              self.size(),
                                                              self.checksum)

    def size(self):
        return self.scale / self.design_size

    def char(self, c):
        return self.font.finfo[c]

    def basic_char(self, c):        
        return self.font.bfinfo[c]

if __name__=='__main__':
    f = TFMFont('cmr12')
    print(f)
    print(f.finfo[30])
    print(f.finfo[31])
    print(f.finfo[32])
    # f = Font('cmr12', 786432, 1, 1)
    # for i, j in f.chars.iteritems():
    #     print chr(i), j
