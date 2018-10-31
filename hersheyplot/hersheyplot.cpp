#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <plot.h>
#include <unistd.h>

#include "hersheyplot.hpp"

using namespace std;
/*

*/
double y_max;


int main (int argc, char **argv)
{
    struct plotinfo pi = {"ps", "a", "none", false, 8.5, 11};
    string action = "limits";

    int c;
    while ((c = getopt(argc, argv, "a:f:p:o:lh")) != -1) {
        switch (c) {
            case 'a':
                action = optarg;
            case 'f':
                pi.format = optarg;
                break;
            case 'p':
                pi.pagesize = optarg;
                break;
            case 'o':
                pi.outfile = optarg;
                break;
            case 'l':
                pi.landscape = true;
                break;
            case 'h':
                on_help(argv[0]);
                exit (EXIT_SUCCESS);
                break;
            default:
                on_help(argv[0]);
                on_error("No such option.");
        }
    }

    PlotPage p(pi);

    p.fbox(0.5, 0.5, p.m_width-0.5, p.m_height-0.5);
    //init_plotinfo(&pi);
    // plPlotter *plotter = create_plotter(&pi);

    // if (action == "limits") {
    //     do_limits(plotter, pi);
    // } else if (action == "hershey") {
    //     do_testplot(plotter, pi.width, pi.height);
    // } else {
    //     on_error("function not implemented");
    // }

    // cleanup(plotter);
    return 0;
}

void on_error(string msg) {
    fprintf (stderr, "%s\n", msg.c_str());
    exit (EXIT_FAILURE);
}

void on_help(string progname) {
    printf ("Usage: %s [options]\n", progname.c_str());
    printf ("\t-a <action>  \t Select action: [ limits, hershey, wordsearch ], default is 'limits'.\n");
    printf ("\t-f <format>  \t Choose format: [ X, png, gif, ps, svg, hpgl ], default is 'ps'.\n");
    printf ("\t-p <pagesize>\t Set page size: [ a, b, c, d, e ], default is 'a'.\n");
    printf ("\t-l           \t Set landscape mode, default is portrait.\n");
    printf ("\t-o <filname> \t Output to file, default is stdout.\n");
    printf ("\t-h           \t This help text.\n");
}

PlotPage::PlotPage(plotinfo const& pi) {
    init_dimensions(pi);
    init_plotter(pi);
}

PlotPage::~PlotPage()
{
    if (m_plotter) {
        delete m_plotter;
    }
}

void PlotPage::init_dimensions(plotinfo const& p) {
    if (p.landscape) {
        switch (p.pagesize[0]) {
            case 'a':
                m_width = 11; m_height = 8.5;
                break;
            case 'b':
                m_width = 17;  m_height = 11;
                break;
            case 'c':
                m_width = 22;  m_height = 17;
                break;
            case 'd':
                m_width = 34;  m_height = 22;
                break;
            case 'e':
                m_width = 44;  m_height = 34;
                break;
            default:
                on_error("No such pagesize");
        }
    } else {
        switch (p.pagesize[0]) {
            case 'a':
                m_width = 8.5; m_height = 11;
                break;
            case 'b':
                m_width = 11;  m_height = 17;
                break;
            case 'c':
                m_width = 17;  m_height = 22;
                break;
            case 'd':
                m_width = 22;  m_height = 34;
                break;
            case 'e':
                m_width = 34;  m_height = 44;
                break;
            default:
                on_error("No such pagesize");
        }
    }
}

void PlotPage::init_plotter(plotinfo const& p) {
    PlotterParams params;
    char desc[100];

    if ((p.format == "ps") || (p.format == "svg"))
    {
        sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xorigin=0,yorigin=0", p.pagesize.c_str(), m_width, m_height);
        fprintf(stderr, "%s\n", desc);
        params.setplparam("PAGESIZE", desc);
        m_plotter = new PSPlotter(cin, cout, cerr, params);
    }
    else if (p.format == "hpgl")
    {
        if (p.landscape) {
            sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
                p.pagesize.c_str(), m_width, m_height, m_width/2.0, m_height/2.0);
        } else {
            sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
                p.pagesize.c_str(), m_width, m_height, m_width/2.0, m_height/2.0);
            params.setplparam("HPGL_ROTATE", const_cast<char*>("90"));
        }
        fprintf(stderr, "%s\n", desc);
        params.setplparam("PAGESIZE", desc);
        params.setplparam("HPGL_VERSION", const_cast<char*>("1"));
        m_plotter = new HPGLPlotter(cin, cout, cerr, params);
    }
    else if ((p.format == "X") || (p.format == "png") || (p.format == "gif"))
    {
        sprintf(desc, "%dx%d", (int)(m_width*50), (int)(m_height*50));
        params.setplparam("BITMAPSIZE", desc);
        fprintf(stderr, "%s\n", desc);
        m_plotter = new XPlotter(cin, cout, cerr, params);
    }
    else
    {
        on_error("Unknown format.");
    }

    /* create a Postscript Plotter that writes to standard output */
    /*if ((plotter = pl_newpl_r ("X", stdin, stdout, stderr, */
    if ( m_plotter->openpl() < 0) {
        on_error("Couldn't create Plotter.");
    }
    m_plotter->fspace(0.0, 0.0, m_width, m_height);
//     pl_erase_r (plotter);
}

double PlotPage::y(double in) {
    return m_height - in;
}

void PlotPage::fbox(double x0, double y0, double x1, double y1) {
    m_plotter->fline(x0, y(y0), x1, y(y0));
    m_plotter->fline(x1, y(y0), x1, y(y1));
    m_plotter->fline(x1, y(y1), x0, y(y1));
    m_plotter->fline(x0, y(y1), x0, y(y0));
}

// void init_plotinfo(struct plotinfo *p) {
//     if (p->landscape) {
//         switch (p->pagesize[0]) {
//             case 'a':
//                 p->width = 11; p->height = 8.5;
//                 break;
//             case 'b':
//                 p->width = 17;  p->height = 11;
//                 break;
//             case 'c':
//                 p->width = 22;  p->height = 17;
//                 break;
//             case 'd':
//                 p->width = 34;  p->height = 22;
//                 break;
//             case 'e':
//                 p->width = 44;  p->height = 34;
//                 break;
//             default:
//                 on_error("No such pagesize");
//         }
//     } else {
//         switch (p->pagesize[0]) {
//             case 'a':
//                 p->width = 8.5; p->height = 11;
//                 break;
//             case 'b':
//                 p->width = 11;  p->height = 17;
//                 break;
//             case 'c':
//                 p->width = 17;  p->height = 22;
//                 break;
//             case 'd':
//                 p->width = 22;  p->height = 34;
//                 break;
//             case 'e':
//                 p->width = 34;  p->height = 44;
//                 break;
//             default:
//                 on_error("No such pagesize");
//         }
//     }
//     y_max = p->height;
// }

// plPlotter* create_plotter(struct plotinfo *p) {
//     plPlotter *plotter;
//     plPlotterParams *plotter_params;
//     char desc[100];


//     /* set a Plotter parameter */
//     plotter_params = pl_newplparams ();

//     // if (p->landscape) {
//     //     pl_setplparam (plotter_params, "ROTATION", "90");
//     // }

//     if ((p->format == "ps") || (p->format == "svg"))
//     {
//         sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xorigin=0,yorigin=0", p->pagesize.c_str(), p->width, p->height);
//         fprintf(stderr, "%s\n", desc);
//         pl_setplparam (plotter_params, "PAGESIZE", desc);
//     }
//     else if (p->format, "hpgl")
//     {
//         if (p->landscape) {
//             sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
//                 p->pagesize.c_str(), p->width, p->height, p->width/2.0, p->height/2.0);
//         } else {
//             sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
//                 p->pagesize.c_str(), p->width, p->height, p->width/2.0, p->height/2.0);
//             pl_setplparam (plotter_params, "HPGL_ROTATE", const_cast<char*>("90"));
//         }
//         fprintf(stderr, "%s\n", desc);
//         pl_setplparam (plotter_params, "PAGESIZE", desc);
//         pl_setplparam (plotter_params, "HPGL_VERSION", const_cast<char*>("1"));

//     }
//     else if ((p->format, "X") || (p->format == "png") || (p->format == "gif"))
//     {
//         sprintf(desc, "%dx%d", (int)(p->width*100), (int)(p->height*100));
//         pl_setplparam (plotter_params, "BITMAPSIZE", desc);
//     }
//     else
//     {
//         on_error("Unknown format.");
//     }

//     /* create a Postscript Plotter that writes to standard output */
//     /*if ((plotter = pl_newpl_r ("X", stdin, stdout, stderr, */
//     if ((plotter = pl_newpl_r (p->format.c_str(), stdin, stdout, stderr,
//                                plotter_params)) == NULL) {
//         on_error("Couldn't create Plotter.");
//     }

//     return plotter;
// }

// double y(double old) {
//     return y_max - old;
// }

// void do_limits(plPlotter *plotter, struct plotinfo p) {

//     char coords[20];

//     if (pl_openpl_r (plotter) < 0) {
//         on_error("Couldn't open Plotter.");
//     }

//     pl_fspace_r (plotter, 0.0, 0.0, p.width, p.height);
//     pl_erase_r (plotter);
//     pl_fontname_r (plotter, "HersheySerif");
//     pl_ffontsize_r (plotter, 0.25);

//     //pl_fbox_r (plotter, 0.5, y(0.5), p.width-0.5, y(p.height-0.5));
//     pl_fline_r (plotter, 0.5, y(0.5), p.width-0.5, y(0.5));
//     pl_fline_r (plotter, p.width-0.5, y(0.5), p.width-0.5, y(p.height-0.5));
//     pl_fline_r (plotter, p.width-0.5, y(p.height-0.5), 0.5, y(p.height-0.5));
//     pl_fline_r (plotter, 0.5, y(p.height-0.5), 0.5, y(0.5));


//     pl_fmove_r (plotter, 1, y(1));
//     sprintf(coords, "(%0.1f, %0.1f)", 0.0, 0.0 );
//     pl_alabel_r(plotter, 'l', 'c', coords);

//     pl_fmove_r (plotter, 1, y(p.height-1));
//     sprintf(coords, "(%0.1f, %0.1f)", 0.0, p.height );
//     pl_alabel_r(plotter, 'l', 'c', coords);

//     pl_fmove_r (plotter, p.width-1, y(1));
//     sprintf(coords, "(%0.1f, %0.1f)", p.width, 0.0 );
//     pl_alabel_r(plotter, 'r', 'c', coords);


//     pl_fmove_r (plotter, p.width-1, y(p.height-1));
//     sprintf(coords, "(%0.1f, %0.1f)", p.width, p.height );
//     pl_alabel_r(plotter, 'r', 'c', coords);


//     if (pl_closepl_r (plotter) < 0) {
//         on_error("Couldn't close Plotter.");
//     }
// }


// void do_testplot(plPlotter *plotter, double x_max, double y_max) {
//     int c = 1;
//     char s[9];
//     char num[5];
//     double xorigin = 0.5;
//     double yorigin = 0.25;
//     double space = 0.5;
//     int ncols = (x_max - xorigin*2) / space;
//     int nrows = (y_max - yorigin*2) / space;

//     while (c < 4399) {

//         if (pl_openpl_r (plotter) < 0) {
//             on_error("Couldn't open Plotter.");
//         }

//         pl_fspace_r (plotter, 0.0, 0.0, x_max, y_max);
//         pl_erase_r (plotter);
//         pl_fontname_r (plotter, "HersheySerif");

//         for (int y=0; y<nrows; y++) {
//             for (int x=0; x<ncols; x++) {
//                 double xloc = xorigin+x*space;
//                 double yloc = y_max - (yorigin+y*space);
//                 //printf("%f, %f\n", xloc, yloc);
//                 pl_fmove_r (plotter, xloc, yloc);

//                 sprintf(s, "\\#H%04d", c);

//                 while (pl_flabelwidth_r(plotter, s) < 0.000001) {
//                     c++;
//                     sprintf(s, "\\#H%04d", c);
//                 }

//                 if (c < 4399) {
//                     pl_ffontsize_r (plotter, 0.35);
//                     pl_alabel_r(plotter, 'c', 'c', s);
//                     pl_fmoverel_r(plotter, 0, -0.2);
//                     pl_ffontsize_r (plotter, 0.07);
//                     sprintf(num, "%04d", c);
//                     pl_alabel_r(plotter, 'c', 'c', num);
//                 }

//                 c++;
//             }
//         }


//         if (pl_closepl_r (plotter) < 0) {
//             on_error("Couldn't close Plotter.");
//         }
//     }
// }


// void cleanup(plPlotter *plotter) {
//     if (pl_deletepl_r (plotter) < 0) {
//         on_error("Couldn't delete Plotter.");
//     }
// }
