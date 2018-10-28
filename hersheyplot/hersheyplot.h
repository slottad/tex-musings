/* hersheyplot.h */

void on_error(char *msg);

plPlotter* create_plotter(char *format, float width, float height);

void do_plot(plPlotter *plotter, float x_max, float y_max);

void cleanup(plPlotter *plotter);
