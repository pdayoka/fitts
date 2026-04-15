#ifndef FITTSMODEL_H
#define FITTSMODEL_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QPoint>

class FittsView;
class FittsController;

class FittsModel {
public:
	FittsModel();
	QJsonObject writeDataJson();
	QJsonObject QPointToQJsonobject(QPoint);

private:
	int cibleLeft = 0;
	int nbCible = 10;
	int minSize = 10;
	int maxSize = 160;

	double a = 0.20;
	double b = 0.10;

	double ecartType = 0;
	double erreurType = 0;
	double diffMoy = 0;
	double itc95 = 0;
    double avgTime = 0;

	QList<QPoint> clickPoints;
	QList<QPoint> cercleCenter;
	QList<int> cercleSize;
	QList<qint64> times;

	friend FittsView;
	friend FittsController;
};

#endif // FITTSMODEL_H
