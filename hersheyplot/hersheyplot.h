/* hersheyplot.h */

void on_error(char *msg);
void on_help();

struct plotinfo
{
	char *format;
	char *pagesize;
	char *outfile;
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
