#include <stdlib.h>
#include <stdio.h>
#include <plot.h>
#include <string.h>
#include <unistd.h>

#include "hersheyplot.h"

/*

*/

int main (int argc, char **argv)
{
    struct plotinfo pi = {"ps", "a", "none", 8.5, 11};
    // char *format = "ps";
    // char *pagesize = "a";
    // char *outfile;
    int c;
    // float x_size = 8.5;
    // float y_size = 11.0;
    // float x_size = 17;
    // float y_size = 22;

    while ((c = getopt(argc, argv, "T:p:o:")) != -1) {
        switch (c) {
            case 'T':
                pi.format = optarg;
                break;
            case 'p':
                pi.pagesize = optarg;
                break;
            case 'o':
                pi.outfile = optarg;
                break;
            default:
                on_error("No such option.");
        }
    }

    init_plotinfo(&pi);
    
    plPlotter *plotter = create_plotter(&pi);

    do_plot(plotter, pi.width, pi.height);

    cleanup(plotter);

    return 0;
}

void on_error(char *msg) {
    fprintf (stderr, "%s\n", msg);
    exit (EXIT_FAILURE);
}

void init_plotinfo(struct plotinfo *p) {
    switch (p->pagesize[0]) {
        case 'a':
            p->width = 8.5;
            p->height = 11;
            break;
        case 'b':
            p->width = 11;
            p->height = 17;
            break;
        case 'c':
            p->width = 17;
            p->height = 22;
            break;
        case 'd':
            p->width = 22;
            p->height = 34;
            break;
        case 'e':
            p->width = 34;
            p->height = 44;
            break;
        default:
            on_error("No such pagesize");
    }
}

plPlotter* create_plotter(struct plotinfo *p) {
    plPlotter *plotter;
    plPlotterParams *plotter_params;
    char desc[100];


    /* set a Plotter parameter */
    plotter_params = pl_newplparams ();

    if (strncmp(p->format, "ps", 2) == 0) {
        sprintf(desc, "%s,xsize=%0.0fin,ysize=%0.0fin,xorigin=0,yorigin=0", p->pagesize, p->width, p->height);
        fprintf(stderr, "%s\n", desc);
        pl_setplparam (plotter_params, "PAGESIZE", desc);
    } else if (strncmp(p->format, "X", 2) == 0) {
       pl_setplparam (plotter_params, "BITMAPSIZE", "850x1100");
    } else {
        on_error("Unknown format.");
    }

    /* create a Postscript Plotter that writes to standard output */
    /*if ((plotter = pl_newpl_r ("X", stdin, stdout, stderr, */
    if ((plotter = pl_newpl_r (p->format, stdin, stdout, stderr,
                               plotter_params)) == NULL) {
        on_error("Couldn't create Plotter.");
    }

    return plotter;
}

void do_plot(plPlotter *plotter, float x_max, float y_max) {
    int c = 1;
    char s[9];
    char num[5];
    float xorigin = 0.5;
    float yorigin = 0.25;
    float space = 0.5;
    int ncols = (x_max - xorigin*2) / space;
    int nrows = (y_max - yorigin*2) / space;

    while (c < 4399) {

        if (pl_openpl_r (plotter) < 0) {
            on_error("Couldn't open Plotter.");
        }

        pl_fspace_r (plotter, 0.0, 0.0, x_max, y_max);
        pl_erase_r (plotter);
        pl_fontname_r (plotter, "HersheySerif");

        for (int y=0; y<nrows; y++) {
            for (int x=0; x<ncols; x++) {
                float xloc = xorigin+x*space;
                float yloc = y_max - (yorigin+y*space);
                //printf("%f, %f\n", xloc, yloc);
                pl_fmove_r (plotter, xloc, yloc);

                sprintf(s, "\\#H%04d", c);

                while (pl_flabelwidth_r(plotter, s) < 0.000001) {
                    c++;
                    sprintf(s, "\\#H%04d", c);
                }

                if (c < 4399) {
                    pl_ffontsize_r (plotter, 0.35);
                    pl_alabel_r(plotter, 'c', 'c', s);
                    pl_fmoverel_r(plotter, 0, -0.2);
                    pl_ffontsize_r (plotter, 0.07);
                    sprintf(num, "%04d", c);
                    pl_alabel_r(plotter, 'c', 'c', num);
                }

                c++;
            }
        }


        if (pl_closepl_r (plotter) < 0) {
            on_error("Couldn't close Plotter.");
        }
    }
}


void cleanup(plPlotter *plotter) {
    if (pl_deletepl_r (plotter) < 0) {
        on_error("Couldn't delete Plotter.");
    }
}
