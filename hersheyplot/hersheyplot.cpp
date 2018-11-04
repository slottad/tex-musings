#include <stdlib.h>
#include <stdbool.h>
//#include <plot.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <cstdio>

#include "hersheyplot.hpp"

using namespace std;
/*

*/
double y_max;

namespace po = boost::program_options;

int main (int argc, char *argv[])
{
    struct plotinfo pi = {"ps", "a", "none", false};
    string action("limits");

    try {
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "This Help message")
            ("action,a", po::value<string>(&action), "What should this program do?")
            ("format,f", po::value<string>(&pi.format), "Output format type.")
            ("pagesize,p", po::value<string>(&pi.pagesize), "Nominal pagesize.")
            ("landscape,l", "Turn on landscape mode.")
            ("outfile,o", po::value<string>(&pi.outfile), "Name (or basename if hpgl) of output file.")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        pi.landscape = vm.count("landscape");
    } catch (const po::error &ex) {
        on_error(ex.what());
    }

    if (action == "limits") {
        plot_limits(pi);
    } else if (action == "hershey") {
        plot_font(pi);
    } else if (action == "fontdim") {
        plot_fontdim(pi);
    } else {
        on_error("function not implemented");
    }

    return 0;
}

void on_error(string msg) {
    cerr << msg << endl;
    exit (EXIT_FAILURE);
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

}

void PlotPage::open() {
    if ( m_plotter->openpl() < 0) {
        on_error("Couldn't open Plotter page.");
    }
    m_plotter->erase();
    m_plotter->fspace(0.0, 0.0, width, height);
}

void PlotPage::close() {
    if ( m_plotter->closepl() < 0) {
        on_error("Couldn't close Plotter page.");
    }
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

void PlotPage::fontname(string name) {
    m_plotter->fontname(name.c_str());
}

void PlotPage::fontsize(double size) {
    m_plotter->ffontsize(size);
}

void PlotPage::move(double x, double y) {
    m_plotter->fmove(x, flip(y));
}

void PlotPage::moverel(double x, double y) {
    m_plotter->fmoverel(x, -y);
}

double PlotPage::labelwidth(string label) {
    return m_plotter->flabelwidth(label.c_str());
}

void plot_limits(plotinfo &pi) {
    PlotPage plotter(pi);

    char coords[20];

    plotter.open();

    plotter.fontname("HersheySans");
    plotter.fontsize(0.25);

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

    plotter.close();
}


void plot_font(plotinfo &pi) {
    PlotPage plotter(pi);

    int c = 1;
    char s[9];
    char num[5];
    double xorigin = 0.5;
    double yorigin = 0.5;
    double space = 1.25;
    int ncols = (plotter.width - xorigin*2) / space;
    int nrows = (plotter.height - yorigin*2) / space;

    while (c < 4399) {

        plotter.open();
        plotter.fontname("HersheySans");

        for (int y=0; y<nrows; y++) {
            for (int x=0; x<ncols; x++) {
                double xloc = xorigin+x*space;
                double yloc = yorigin+y*space;
                //fprintf(stderr, "%f, %f\n", xloc, yloc);
                plotter.move(xloc, yloc);

                sprintf(s, "\\#H%04d", c);
                //fprintf(stderr, "%s\n", s);

                while (plotter.labelwidth(s) < 0.000001) {
                    c++;
                    sprintf(s, "\\#H%04d", c);
                    // fprintf(stderr, "%s\n", s);
                }

                if (c < 4399) {
                    plotter.fontsize(0.75);
                    plotter.label('c', 'c', s);
                    plotter.moverel(0, 0.6);
                    plotter.fontsize(0.25);
                    sprintf(num, "%04d", c);
                    plotter.label('c', 'c', num);
                }

                c++;
            }
        }
        plotter.close();
    }
}


void plot_fontdim(plotinfo &pi) {
    //PlotPage plotter(pi);

    vector<string> fontlist = { "HersheySerif",
                                "HersheySerif-Italic",
                                "HersheySerif-Bold",
                                "HersheySerif-BoldItalic",
                                "HersheySans",
                                "HersheySans-Oblique",
                                "HersheySans-Bold",
                                "HersheySans-BoldOblique",
                                "HersheyScript",
                                "HersheyScript-Bold" };
    for (auto f : fontlist) {
        cout << f << endl;
    }

}
