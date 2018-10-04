#!/usr/bin/env python
from __future__ import print_function
from dvipy.read import read_pre, read_bop, read_eop, DviState
from dvipy.fontmap import fntmap, load_requested, get_encoding
from dvipy.sizer import TexSizer

import itertools
import logging
from collections import namedtuple
logger = logging.getLogger(__name__)

def repeat_func(f, *args):
    """Repeatedly apply a function, returning an iterator of the results"""
    return itertools.starmap(f, itertools.repeat(args))

# The scaling does need to be done somewhere, to make sure that fonts etc. are
# the right. It would also be nice to have a test bed somewhere that can just

DVI2PT = 2**-16

# When we sit and think for a while, the only thing we can have happen is a rule
# or a char, even with virtual fonts taken into consideration (yeah, this
# neglects specials)

Rule = namedtuple('Rule', ('x', 'y', 'w', 'h'))
Char = namedtuple('Char', ('x', 'y', 'char', 'font'))
Font = namedtuple('Font', ('filename', 'size'))


class T1Font(object):
    def __init__(self, filename, font, enc, scale=1.0):
        self.enc = enc
        self.fontcore = Font(filename, scale * font.size)

    def render(self, renderer, state, char):
        if self.enc:
            char = self.enc[char]
        c = Char(DVI2TP * state.h, DVI2TP * state.v, char, self.fontcore)
        renderer.page.append(c)

class DviSlave():
    # Since we ignore specials and all that nonsense, we only need to store a
    # list of fonts, characters, and their positions. We can use a dispatcher
    # pattern to make this work -- have a heterogenous list with various objects
    # representing the renderings to perform. So really, all this does is handle
    # virtual fonts and compile things down into DVI objects. So for instance,
    # there will be lots and lots of tiny glyph objects.
    def __init__(self):
        self.fonts = {}
        self.page = []
        self.sizer = TexSizer()
        self.in_vf = False

    @property
    def callbacks(self):
        print("callbacks")
        return dict(
            char = self.on_put_char,
            rule = self.on_put_rule,
            fnt = self.on_fnt,
            fnt_def = self.on_fnt_def
        )

    def clear_page(self):
        page = self.page
        (w, h), (b, l) = self.sizer.reset()
        size = DVI2TP * w, DVI2TP * h
        bl = DVI2TP * b, DVI2TP * l
        self.page = []
        print(size, bl)
        return DviPage(page, size, bl)

    def on_put_char(self, state, i):
        self.font.render(self, state, i)
        # But if it's a virtual font how do we tell? ARRGH!
        # After about ~1min of thinking, this inelegant solution was proposed!
        if not self.in_vf:
            self.sizer.on_put_char(state, i)

    def on_fnt_def(self, texfont):
        r = fntmap.get(texfont.name)
        if r:
            pfbs, encs = load_requested(r)
            psname = pfbs[0]
            enc = get_encoding(encs[-1]) if encs else None
            font = T1Font(psname, texfont, enc)
        else:
            vffile = kpse('%s.vf' % texfont.name)
            font = VirtualFont(vffile, texfont)

        self.fonts[texfont.n] = font

    def on_fnt(self, state, fnt, k):
        self.font = self.fonts[k]
        print(self.font)

    def on_put_rule(self, s, a, b):
        self.page.append(Rule(DVI2PT * s.h, DVI2PT * s.v,
                              DVI2PT * b, DVI2PT * a))
        self.sizer.on_put_rule(s, a, b)
        print("rule", s, a, b)


# Read the dvi file
def read_dvi(filename, debug=logging.debug):
    # Magic happens here
    #print "opening..."
    with open(filename, 'rb') as f:
        debug('Opened %s for reading' % filename)
        state = DviState()
        slave = DviSlave()
        state.set_callbacks(**slave.callbacks)

        #import pdb; pdb.set_trace()
        try:
            s = itertools.imap(ord, repeat_func(f.read, 1))
        except:
            return

        # Let's read the preamble
        read_pre(s, state)
        debug("Read preamble")

        while True:
            # Let's wait...
            #if not order_queue.get():
            #    break

            read_bop(s, state)
            read_eop(s, state)

            #out_queue.put(slave.clear_page())
            #order_queue.task_done()
            debug("Read page")

        #order_queue.task_done()
        debug('Terminate reader')

if __name__=='__main__':
    logging.basicConfig(filename='log.txt', filemode='w', level=logging.DEBUG)
    read_dvi("math.cmr.utf8.dvi")
