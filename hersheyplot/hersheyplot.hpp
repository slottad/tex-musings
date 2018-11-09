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
	double fontsize;
    bool landscape;
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

	void open();
	void close();

	// Drawing functions

	void box(double x0, double y0, double x1, double y1);
	void boxrel(double x0, double y0, double x1, double y1);
	void line(double x0, double y0, double x1, double y1);
	void linerel(double x0, double y0, double x1, double y1);

	void label(int hjust, int vjust, string text);

	// Settings

	void fontname(string name);
	void fontsize(double size);
	void move(double x, double y);
	void moverel(double x, double y);

	// Query
	double labelwidth(string label);

private:

	double flip(double in);
};


void plot_limits(plotinfo &pi);
void plot_hershey(plotinfo &pi);
void plot_fontdim(plotinfo &pi);
void plot_tfm(plotinfo &pi);
