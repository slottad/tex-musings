/* hersheyplot.h */

void on_error(char *msg);

struct plotinfo
{
	char *format;
	char *pagesize;
	char *outfile;
	float width;
	float height;
};


void init_plotinfo(struct plotinfo *p);
plPlotter* create_plotter(struct plotinfo *p);

void do_plot(plPlotter *plotter, float x_max, float y_max);

void cleanup(plPlotter *plotter);
