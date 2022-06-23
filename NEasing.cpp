#include "NEasing.h"

double EaseOut(double x, int i) {
	return 1 - pow(1 - x, i);
}

double EaseIn(double x) {
	return pow(x, 4);
}

double EaseOutSine(double x) {
	return 1 - sin((x * PI) / 2);
}

double EaseInSine(double x) {
	return 1 - cos((x * PI) / 2);
}

double EaseInOutBack(double x) {
	const double c1 = 1.70158;
	const double c2 = c1 * 1.525;

	return x < 0.5
		? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
		: (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}