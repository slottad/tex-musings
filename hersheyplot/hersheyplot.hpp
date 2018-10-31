/* hersheyplot.h */
#include <string>
#include <plotter.h>

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

class PlotPage
{
	Plotter *m_plotter;

public:

	double width;
	double height;


	PlotPage(plotinfo const& pi);

	~PlotPage();

	// Initialization code
	void init_dimensions(plotinfo const& pi);
	void init_plotter(plotinfo const& pi);

	// Drawing functions

	void fbox(double x0, double y0, double x1, double y1);
	void label(int hjust, int vjust, string text);

	// Settings

	void set_font(string name, double size);
	void move(double x, double y);

private:

	double flip(double in);
};


// void init_plotinfo(struct plotinfo *p);
// plPlotter* create_plotter(struct plotinfo *p);

// double y(double oldy);

void do_limits(PlotPage &plotter);
// void do_testplot(plPlotter *plotter, double x_max, double y_max);

// void cleanup(plPlotter *plotter);
