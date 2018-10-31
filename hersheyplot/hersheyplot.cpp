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

    PlotPage plotter(pi);

    //p.fbox(0.5, 0.5, p.m_width-0.5, p.m_height-0.5);
    //init_plotinfo(&pi);
    // plPlotter *plotter = create_plotter(&pi);

    if (action == "limits") {
        do_limits(plotter);
    // } else if (action == "hershey") {
    //     do_testplot(plotter, pi.width, pi.height);
    } else {
        on_error("function not implemented");
    }

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
                width = 11; height = 8.5;
                break;
            case 'b':
                width = 17;  height = 11;
                break;
            case 'c':
                width = 22;  height = 17;
                break;
            case 'd':
                width = 34;  height = 22;
                break;
            case 'e':
                width = 44;  height = 34;
                break;
            default:
                on_error("No such pagesize");
        }
    } else {
        switch (p.pagesize[0]) {
            case 'a':
                width = 8.5; height = 11;
                break;
            case 'b':
                width = 11;  height = 17;
                break;
            case 'c':
                width = 17;  height = 22;
                break;
            case 'd':
                width = 22;  height = 34;
                break;
            case 'e':
                width = 34;  height = 44;
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
        sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xorigin=0,yorigin=0", p.pagesize.c_str(), width, height);
        fprintf(stderr, "%s\n", desc);
        params.setplparam("PAGESIZE", desc);
        m_plotter = new PSPlotter(cin, cout, cerr, params);
    }
    else if (p.format == "hpgl")
    {
        if (p.landscape) {
            sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
                p.pagesize.c_str(), width, height, width/2.0, height/2.0);
        } else {
            sprintf(desc, "%s,xsize=%0.1fin,ysize=%0.1fin,xoffset=-%0.2fin,yoffset=-%0.2fin",
                p.pagesize.c_str(), width, height, width/2.0, height/2.0);
            params.setplparam("HPGL_ROTATE", const_cast<char*>("90"));
        }
        fprintf(stderr, "%s\n", desc);
        params.setplparam("PAGESIZE", desc);
        params.setplparam("HPGL_VERSION", const_cast<char*>("1"));
        m_plotter = new HPGLPlotter(cin, cout, cerr, params);
    }
    else if ((p.format == "X") || (p.format == "png") || (p.format == "gif"))
    {
        sprintf(desc, "%dx%d", (int)(width*50), (int)(height*50));
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
    m_plotter->fspace(0.0, 0.0, width, height);
    m_plotter->erase();
}

double PlotPage::flip(double in) {
    return height - in;
}

void PlotPage::fbox(double x0, double y0, double x1, double y1) {
    m_plotter->fline(x0, flip(y0), x1, flip(y0));
    m_plotter->fline(x1, flip(y0), x1, flip(y1));
    m_plotter->fline(x1, flip(y1), x0, flip(y1));
    m_plotter->fline(x0, flip(y1), x0, flip(y0));
}

void PlotPage::label(int hjust, int vjust, string text) {
    m_plotter->alabel(hjust, vjust, text.c_str());
}

void PlotPage::set_font(string name, double size) {
    m_plotter->fontname(name.c_str());
    m_plotter->ffontsize(size);
}

void PlotPage::move(double x, double y) {
    m_plotter->fmove(x, flip(y));
}

void do_limits(PlotPage &plotter) {

    char coords[20];

    plotter.set_font("HersheySerif", 0.25);
    plotter.fbox(0.5, 0.5, plotter.width-0.5, plotter.height-0.5);

    plotter.move(1, 1);
    sprintf(coords, "(%0.1f, %0.1f)", 0.0, 0.0 );
    plotter.label('l', 'c', coords);

    plotter.move(1, plotter.height-1);
    sprintf(coords, "(%0.1f, %0.1f)", 0.0, plotter.height );
    plotter.label('l', 'c', coords);

    plotter.move(plotter.width-1, 1);
    sprintf(coords, "(%0.1f, %0.1f)", plotter.width, 0.0 );
    plotter.label('r', 'c', coords);


    plotter.move(plotter.width-1, plotter.height-1);
    sprintf(coords, "(%0.1f, %0.1f)", plotter.width, plotter.height );
    plotter.label('r', 'c', coords);
}


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
