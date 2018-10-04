#include <stdio.h>
#include <plot.h>
#define MAXORDER 12

/*
Compile with:

    gcc -o hersheyplot -lplot -lXaw -lXmu -lXt -lSM -lICE -lXext -lX11 -lpng -lz -lm hersheyplot.c
*/

void draw_c_curve (plPlotter *plotter, double dx, double dy, int order)
{

    if (order >= MAXORDER)
        /* continue path along (dx, dy) */
        pl_fcontrel_r (plotter, dx, dy);
    else {
        draw_c_curve (plotter, 0.5 * (dx - dy), 0.5 * (dx + dy), order + 1);
        draw_c_curve (plotter, 0.5 * (dx + dy), 0.5 * (dy - dx), order + 1);
    }
}


int main ()
{

    plPlotter *plotter;

    plPlotterParams *plotter_params;


    /* set a Plotter parameter */
    plotter_params = pl_newplparams ();

    pl_setplparam (plotter_params, "PAGESIZE", "letter");
    pl_setplparam (plotter_params, "BITMAPSIZE", "850x1100");


    /* create a Postscript Plotter that writes to standard output */
    /*if ((plotter = pl_newpl_r ("X", stdin, stdout, stderr, */
    if ((plotter = pl_newpl_r ("ps", stdin, stdout, stderr,
                               plotter_params)) == NULL) {
        fprintf (stderr, "Couldn't create Plotter\n");
        return 1;
    }

    int c = 1;
    char s[9];
    char num[5];
    float X_MAX = 8.5;
    float Y_MAX = 11.0;
    float xorigin = 0.5;
    float yorigin = 0.25;
    float space = 0.5;
    int ncols = (X_MAX - xorigin*2) / space;
    int nrows = (Y_MAX - yorigin*2) / space;
        
    while (c < 4399) {
        
        if (pl_openpl_r (plotter) < 0) {
            fprintf (stderr, "Couldn't open Plotter\n");
            return 1;
        }
    
        pl_fspace_r (plotter, 0.0, 0.0, X_MAX, Y_MAX);
        /* set coor system */
        //pl_flinewidth_r (plotter, 0.25);
        /* set line thickness */
        //pl_pencolorname_r (plotter, "red");
        /* use red pen */
        pl_erase_r (plotter);
        /* erase graphics display */
        /* position the graphics cursor */
        //draw_c_curve (plotter, 0.0, 400.0, 0);
        pl_fontname_r (plotter, "HersheySerif");

    
        for (int y=0; y<nrows; y++) {
            for (int x=0; x<ncols; x++) {
                float xloc = xorigin+x*space;
                float yloc = Y_MAX - (yorigin+y*space);
                //printf("%f, %f\n", xloc, yloc);
                pl_fmove_r (plotter, xloc, yloc);
            
                sprintf(s, "\\#H%04d", c);
                //float w = pl_flabelwidth_r(plotter, s);
                //printf("%d\t%s\t%f\n", c, s, w);
            
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
            fprintf (stderr, "Couldn't close Plotter\n");
            return 1;
        }
    }
    

    if (pl_deletepl_r (plotter) < 0) {
        fprintf (stderr, "Couldn't delete Plotter\n");
        return 1;
    }

    return 0;

}
