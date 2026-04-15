#include "fittsmodel.h"

#include "fittsview.h"

FittsModel::FittsModel() {}

QJsonObject FittsModel::writeDataJson() {
	QJsonObject dataItem;

	dataItem["cibleLeft"] = cibleLeft;
	dataItem["nbCible"] = nbCible;
	dataItem["minSize"] = minSize;
	dataItem["maxSize"] = maxSize;
	dataItem["a"] = a;
	dataItem["b"] = b;
	dataItem["ecartType"] = ecartType;
	dataItem["erreurType"] = erreurType;
	dataItem["diffMoy"] = diffMoy;
	dataItem["itc95"] = itc95;
	dataItem["avgTime"] = avgTime;

	QJsonArray clickPointsArray;

	for (int i = 0; i < clickPoints.size(); i++) {

		clickPointsArray.append(QPointToQJsonobject(clickPoints[i]));
	}

	dataItem["clickPoints"] = clickPointsArray;

	QJsonArray cercleCenterArray;

	for (int i = 0; i < cercleCenter.size(); i++) {

		cercleCenterArray.append(QPointToQJsonobject(cercleCenter[i]));
	}

	dataItem["cercleCenter"] = cercleCenterArray;

	QJsonArray cercleSizeArray;

	for (int i = 0; i < cercleSize.size(); i++) {

		cercleSizeArray.append(cercleSize[i]);
	}

	dataItem["cercleSize"] = cercleSizeArray;

	QDateTime *currentDate = new QDateTime;

	dataItem["dateTime"] =
		currentDate->currentDateTime().toString("dd/MM/yyyy - hh:mm");

	// qWarning() << dataItem;

	return dataItem;
}

QJsonObject FittsModel::QPointToQJsonobject(QPoint point) {

	QJsonObject converter;
	converter.insert("x", point.x());
	converter.insert("y", point.y());

	return converter;
}
