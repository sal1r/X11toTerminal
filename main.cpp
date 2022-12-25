#include <unistd.h>
#include <curses.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <list>
#include <vector>

struct Color {
	int r;
	int g;
	int b;

	Color(int r, int g, int b) : r(r), g(g), b(b) {}
};

short nearestColor(const XColor& c, const std::list<Color>& colors) {
	Color co = colors.front();
	short coCo = 0;
	Color cc((short)((double)c.red /  65535.0 * 1000.0),
	(short)((double)c.green /  65535.0 * 1000.0),
	(short)((double)c.blue /  65535.0 * 1000.0));

	short i = 0;
	for (auto& col : colors) {
		if (sqrt(pow(cc.r - col.r, 2) + pow(cc.g - col.g, 2) + pow(cc.b - col.b, 2)) < 
				sqrt(pow(cc.r - co.r, 2) + pow(cc.g - co.g, 2) + pow(cc.b - co.b, 2))) {
			co = col;
			coCo = i;
		}
		++i;
	}
	return coCo;
}

short nearestColor(const Color& c, const std::list<Color>& colors) {
	Color co = colors.front();
	short coCo = 0;

	short i = 0;
	for (auto& col : colors) {
		if (sqrt(pow(c.r - col.r, 2) + pow(c.g - col.g, 2) + pow(c.b - col.b, 2)) < 
				sqrt(pow(c.r - co.r, 2) + pow(c.g - co.g, 2) + pow(c.b - co.b, 2))) {
			co = col;
			coCo = i;
		}
		++i;
	}
	return coCo;
}

Color averageColor(const std::list<Color>& colors) {
	std::vector<double> cs;
	double ac = 0;

	for (auto& col : colors) {
		cs.push_back(sqrt(pow(col.r, 2) + pow(col.g, 2) + pow(col.b, 2)));
		ac += sqrt(pow(col.r, 2) + pow(col.g, 2) + pow(col.b, 2));
	}

	ac /= 4;

	double c = cs.front();
	Color co = colors.front();
	short i = 0;

	for (auto& cc : cs) {
		if (abs(ac - cc) < abs(ac - c)) {
			auto it = std::next(colors.begin(), i);
			co = *it;
			c = cc;
		}
		i++;
	}
	return co;
}

int main()
{
	initscr();
	refresh();
	noecho();
	curs_set(0);

	// std::list<Color> colors{{1000, 1000, 1000}, {0, 0, 0},
	// 		{1000, 0, 0}, {0, 1000, 0},
	// 		{0, 0, 1000}, {1000, 1000, 0},
	// 		{0, 1000, 1000}, {1000, 0, 1000},
	// 		{300, 300, 300}, {700, 700, 700},
	// 		{100, 100, 100}, {700, 700, 700}};

	std::list<Color> colors;

	short r = 5;
	while (--r >= 0) {

		short g = 5;
		while (--g >= 0) {

			short b = 5;
			while (--b >= 0) {
				colors.push_back(Color(r * 250, g * 250, b * 250));
			}
		}
	}

	start_color();
	
	short i = 0;
	for (auto& c : colors) {
		init_color(i, c.r, c.g, c.b);
		init_pair(i, i, i);
		++i;
	}

	while (1) {
		XColor c;
   		Display *d = XOpenDisplay((char*)NULL);

		int width = 1366;
		int height = 768;

  		XImage *image;
  		image = XGetImage(d, XRootWindow (d, XDefaultScreen (d)), 0, 0, width, height, AllPlanes, XYPixmap);

		double coef = 8.0;

		int y = -1;
		while (++y < floor((double)height / coef)) {

			int x = -1;
			while (++x < floor((double)width / (coef / 2.0))) {

				std::list<Color> cols;
				short yy = -1;
				while (++yy < coef) {

					short xx = -1;
					while (++xx < (coef / 2.0)) {
						c.pixel = XGetPixel(image, x * (coef / 2.0) + xx, y * coef + yy);
						XQueryColor (d, XDefaultColormap(d, XDefaultScreen (d)), &c);
						cols.push_back(Color((short)((double)c.red /  65535.0 * 1000.0),
							(short)((double)c.green /  65535.0 * 1000.0),
							(short)((double)c.blue /  65535.0 * 1000.0)));
					}
				}

				move(y, x);
				addch(' ' | COLOR_PAIR(nearestColor(averageColor(cols), colors)));

				refresh();


				// c.pixel = XGetPixel(image, x, y);
				// XQueryColor (d, XDefaultColormap(d, XDefaultScreen (d)), &c);

				// move(y, x);
				// addch(' ' | COLOR_PAIR(nearestColor(c, colors)));

				// refresh();


				// init_pair(1, COLOR_RED, COLOR_RED);
				// init_pair(2, COLOR_GREEN, COLOR_GREEN);
				// init_pair(3, COLOR_BLUE, COLOR_BLUE);

				// if (c.red >= c.green && c.red >= c.blue) {
				// 	move(y, x);
				// 	addch(' ' | COLOR_PAIR(1));
				// }

				// if (c.green >= c.red && c.green >= c.blue) {
				// 	move(y, x);
				// 	addch(' ' | COLOR_PAIR(2));
				// }

				// if (c.blue >= c.green && c.blue >= c.red) {
				// 	move(y, x);
				// 	addch(' ' | COLOR_PAIR(3));
				// }
			}
		}

   		XFree(image);
		sleep(5);
	}

	endwin();
	return 0;
}