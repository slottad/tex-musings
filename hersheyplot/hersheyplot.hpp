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

	double m_width;
	double m_height;
	

	PlotPage(plotinfo const& pi);

	~PlotPage();

	// Initialization code
	void init_dimensions(plotinfo const& pi);	
	void init_plotter(plotinfo const& pi);	

	// Drawing functions

	void fbox(double x0, double y0, double x1, double y1);

private:

	double y(double in);
};


// void init_plotinfo(struct plotinfo *p);
// plPlotter* create_plotter(struct plotinfo *p);

// double y(double oldy);

// void do_limits(plPlotter *plotter, struct plotinfo p);
// void do_testplot(plPlotter *plotter, double x_max, double y_max);

// void cleanup(plPlotter *plotter);
