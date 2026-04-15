#include "fittscontroller.h"
#include "fittsmodel.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
	QApplication::setAttribute(
		Qt::AA_EnableHighDpiScaling); // Suivre le mode de mise à l'échelle du
	                                  // système

	QApplication a(argc, argv);
	a.setApplicationName("Fitts");
	a.setApplicationDisplayName("Loi de Fitts et Keystroke - HM40");
	a.setAttribute(Qt::AA_UseHighDpiPixmaps); // Affichage correct des SVG lors
	                                          // de la mise à l'échelle

	QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Regular.ttf");
	QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Italic.ttf");
	QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Medium.ttf");
	QFontDatabase::addApplicationFont(
		":/fonts/res/fonts/Roboto-MediumItalic.ttf");
	QFontDatabase::addApplicationFont(":/fonts/res/fonts/Roboto-Bold.ttf");
	QFontDatabase::addApplicationFont(
		":/fonts/res/fonts/Roboto-BoldItalic.ttf");

	FittsController *controller = new FittsController;
	int exitCode = a.exec();
	delete controller;
	return exitCode;
}
