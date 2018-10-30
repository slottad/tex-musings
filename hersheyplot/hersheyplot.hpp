/* hersheyplot.h */
#include <string>

void on_error(std::string msg);
void on_help(std::string progname);

struct plotinfo
{
	std::string format;
	std::string pagesize;
	std::string outfile;
    bool landscape;
	double width;
	double height;
};


void init_plotinfo(struct plotinfo *p);
plPlotter* create_plotter(struct plotinfo *p);

double y(double oldy);

void do_limits(plPlotter *plotter, struct plotinfo p);
void do_testplot(plPlotter *plotter, double x_max, double y_max);

void cleanup(plPlotter *plotter);
