#include "fittscontroller.h"

#include "fittsmodel.h"
#include "fittsview.h"

using namespace std;

FittsController::FittsController() {
	this->fittsModel = new FittsModel();
	this->fittsView = new FittsView(this, this->fittsModel);
	this->histModel = new QList<FittsModel>();

	this->keystrokeModel = new KeystrokeModel;

	this->fittsView->show();
}

FittsController::~FittsController() {
	delete fittsModel;
	delete fittsView;
	delete histModel;
	// delete keystrokeModel;
}

void FittsController::initEvalScreen() {
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->evalScreenWidget);
	this->fittsModel->cibleLeft = this->fittsModel->nbCible;
	this->fittsView->graphicView->setEnabled(true);
	this->fittsModel->cercleSize.clear();
	this->fittsModel->cercleCenter.clear();
	this->fittsModel->clickPoints.clear();
	this->fittsModel->times.clear();
	this->fittsView->updateTestMsg();
	this->initGame();
}

void FittsController::initResultsScreen() {
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->resultsScreenWidget);
	this->calculateResultHome();
	this->addHisto();
	this->displayResults();
}

void FittsController::initSettingsScreen() {
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->settingsScreenWidget);
}

void FittsController::aValueChanged(double value) {//**************************************************
	this->fittsModel->a = value;
    this->calculateResultHome();
    //this->addHisto();
    this->displayResults();
}
void FittsController::bValueChanged(double value) {
	this->fittsModel->b = value;
    this->calculateResultHome();
    //this->addHisto();
    this->displayResults();
}
void FittsController::nbCibleChanged(int value) {
	this->fittsModel->nbCible = value;
    this->calculateResultHome();
    //this->addHisto();
    this->displayResults();
}
void FittsController::minSizeChanged(int value) {
	this->fittsModel->minSize = value;
}
void FittsController::maxSizeChanged(int value) {
	this->fittsModel->maxSize = value;
}
void FittsController::cibleClicked(int x, int y) {
	if (this->fittsModel->cercleCenter.isEmpty()) {
		// Si vide alors premier clic, on demarre le timer
		this->timer = new QElapsedTimer;
		timer->start();

		// On démarre avec la première cible
		this->fittsModel->clickPoints.append(QPoint(x, y));
		this->nextCible();
	} else {
		QPointF coords = this->fittsView->graphicView->mapToScene(x, y);
		if (sqrt(
				pow(coords.x() - this->fittsModel->cercleCenter.last().x(), 2) +
				pow(coords.y() - this->fittsModel->cercleCenter.last().y(),
					2)) <= this->fittsModel->cercleSize.last() / 2) {
			// On stocke le temps de clic
			this->fittsModel->times.append(timer->elapsed());
			// On redémarre le chrono
			timer->restart();

			// On stocke la position du clic
			this->fittsModel->clickPoints.append(QPoint(x, y));
			this->nextCible();
		}
	}
}

void FittsController::nextCible() {
	if (!this->fittsModel->cercleCenter.isEmpty())
		this->fittsModel->cibleLeft--;
	this->fittsView->updateTestMsg();

	QGraphicsScene *scene = this->fittsView->scene;
	scene->clear();

	// On stoppe si c'est fini
	if (this->fittsModel->cibleLeft == 0) {
		this->finish();
		return;
	}

	// On génère la taille du cercle rouge
	int size =
		QRandomGenerator::global()->generate() %
			((this->fittsModel->maxSize + 1) - this->fittsModel->minSize) +
		this->fittsModel->minSize;
	// Car on veut le rayon
	// On place le cercle dans la scène (il ne doit pas être en dehors)
	int sceneW = int(this->fittsView->scene->width());
	int sceneH = int(this->fittsView->scene->height());

	qreal posX = QRandomGenerator::global()->generate() %
	             std::max(((sceneW - size) - size) + size, 1);
	qreal posY = QRandomGenerator::global()->generate() %
	             std::max(((sceneH - size) - size) + size, 1);

	// On stock les infos sur le cercle
	this->fittsModel->cercleCenter.append(QPoint(int(posX), int(posY)));
	this->fittsModel->cercleSize.append(size);

	// On place le cercle
	scene
		->addEllipse(posX - (size / 2), posY - (size / 2), size, size,
			QPen(QColor(color_red)), QBrush(QColor(color_red)))
		->setCursor(Qt::PointingHandCursor);
}

void FittsController::finish() {
	this->fittsView->graphicView->setEnabled(false);
	initResultsScreen();
}

void FittsController::initGame() {
	QGraphicsScene *scene = this->fittsView->scene;
	scene->clear();

	if (this->fittsModel->maxSize >= this->fittsView->graphicView->width() / 2)
		this->fittsModel->maxSize = this->fittsView->graphicView->width() / 2;

	if (this->fittsModel->maxSize >= this->fittsView->graphicView->height() / 2)
		this->fittsModel->maxSize = this->fittsView->graphicView->height() / 2;

	qreal posX = scene->width() / 2;
	qreal posY = scene->height() / 2;
	int size = 180;

	QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
	effect->setBlurRadius(32);
	effect->setXOffset(0);
	effect->setYOffset(0);
	effect->setColor(color_blue);

	scene
		->addEllipse(posX - (size / 2), posY - (size / 2), size, size,
			QPen(QColor(color_blue)), QBrush(QColor(color_blue)))
		->setGraphicsEffect(effect);
	scene
		->addEllipse(posX - (size / 2), posY - (size / 2), size, size,
			QPen(QColor(color_blue)), QBrush(QColor(color_blue)))
		->setCursor(Qt::PointingHandCursor);

	QLabel *labelStart;
	labelStart = new QLabel("Démarrer");
	labelStart->setProperty("class", "label-start");
	labelStart->setStyleSheet("color: #ffffff; font: bold 18px Roboto; "
							  "background-color: transparent;");
	labelStart->autoFillBackground();
	labelStart->setAlignment(Qt::AlignCenter);
	QGraphicsProxyWidget *labelStartProxy = scene->addWidget(labelStart);
	labelStartProxy->setPos((size / 2) + 160, (size / 2) + 28);

	QPropertyAnimation *animation =
		new QPropertyAnimation(effect, "blurRadius");
	animation->setDuration(1500);
	animation->setKeyValueAt(0, 0);
	animation->setKeyValueAt(0.5, 64);
	animation->setKeyValueAt(1, 0);
	animation->setEasingCurve(QEasingCurve::InQuad);
	animation->setLoopCount(-1);
	animation->start();
}

void FittsController::calculateResultHome() {//**************************************************************************
    // Créer de nouveaux graphiques
    QChart *chartTimePerTarget = new QChart;
    QChart *chartTimeFunctionOfDistance = new QChart;

    // Associer les nouveaux charts
    this->fittsView->plotHome->setChart(chartTimePerTarget);
    this->fittsView->plotHome->setRenderHint(QPainter::Antialiasing);
    this->fittsView->plotHomeDistance->setChart(chartTimeFunctionOfDistance);
    this->fittsView->plotHomeDistance->setRenderHint(QPainter::Antialiasing);

    // Paramètres visuels
    chartTimePerTarget->setTitle("Temps par cible");
    chartTimePerTarget->setAnimationOptions(QChart::AllAnimations);
    chartTimePerTarget->legend()->setVisible(false);

    chartTimeFunctionOfDistance->setTitle("Temps en fonction de la distance relative");
    chartTimeFunctionOfDistance->setAnimationOptions(QChart::AllAnimations);
    chartTimeFunctionOfDistance->legend()->setVisible(false);

    // Séries pour le premier graphique
    QLineSeries *expSeries = new QLineSeries;
    expSeries->setName("Courbe expérimentale");
    expSeries->setColor(color_red);

    QLineSeries *fittsSeries = new QLineSeries;
    fittsSeries->setName("Courbe théorique");
    fittsSeries->setColor(color_blue);

    QCategoryAxis *axis = new QCategoryAxis;

    // Séries pour le second graphique
    QScatterSeries *expSeriesDistance = new QScatterSeries;
    expSeriesDistance->setName("Points expérimentaux");
    expSeriesDistance->setColor(color_red);
    expSeriesDistance->setMarkerSize(10);

    QLineSeries *fittsSeriesDistance = new QLineSeries;
    fittsSeriesDistance->setName("Courbe théorique");
    fittsSeriesDistance->setColor(color_blue);

    QCategoryAxis *axisDistance = new QCategoryAxis;

    // Données
    QList<double> fittsValues;
    QList<double> distRelatif;
    QList<double> listeTemps;
    QList<double> listeTempsCalcule;

    int nbPoints = qMin(this->fittsModel->nbCible, this->fittsModel->times.size());//*************************
    for (int i = 0; i < nbPoints; ++i){//*************************************************************************
        double T = this->fittsModel->times[i];
        listeTemps.append(T);
        expSeries->append(i, T);

        double D = sqrt(pow(this->fittsModel->clickPoints[i].x() - this->fittsModel->cercleCenter[i].x(), 2) +
                        pow(this->fittsModel->clickPoints[i].y() - this->fittsModel->cercleCenter[i].y(), 2));
        double L = this->fittsModel->cercleSize[i];
        double F = log2((D / L) + 1);
        expSeriesDistance->append(F, T);

        double value = (this->fittsModel->a * 1000) + (this->fittsModel->b * 1000 * F);
        listeTempsCalcule.append(value);
        fittsValues.append(value);
        fittsSeries->append(i, value);
        fittsSeriesDistance->append(F, value);

        axis->append(QString::number(i + 1), i);
        axisDistance->append(QString::number(F), i);
    }

    // Trier pour lisser les courbes distances
    QVector<QPointF> points = fittsSeriesDistance->pointsVector();
    std::sort(points.begin(), points.end(), [](const QPointF &p1, const QPointF &p2) {
        return p1.x() < p2.x();
    });
    fittsSeriesDistance->replace(points);

    QVector<QPointF> points2 = expSeriesDistance->pointsVector();
    std::sort(points2.begin(), points2.end(), [](const QPointF &p1, const QPointF &p2) {
        return p1.x() < p2.x();
    });
    expSeriesDistance->replace(points2);

    // Ajouter les séries aux graphiques
    chartTimePerTarget->addSeries(expSeries);
    chartTimePerTarget->addSeries(fittsSeries);

    chartTimeFunctionOfDistance->addSeries(expSeriesDistance);
    chartTimeFunctionOfDistance->addSeries(fittsSeriesDistance);

    // Axes
    chartTimePerTarget->createDefaultAxes();
    chartTimePerTarget->setAxisX(axis, expSeries);
    chartTimePerTarget->setAxisX(axis, fittsSeries);

    chartTimeFunctionOfDistance->createDefaultAxes();
    chartTimeFunctionOfDistance->setAxisX(axisDistance, expSeriesDistance);
    chartTimeFunctionOfDistance->setAxisX(axisDistance, fittsSeriesDistance);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("temps (ms)");
    chartTimePerTarget->setAxisY(axisY, expSeries);

    QValueAxis *axisYDistance = new QValueAxis;
    axisYDistance->setTitleText("temps (ms)");
    chartTimeFunctionOfDistance->setAxisY(axisYDistance, expSeriesDistance);

    // Calcul des statistiques
    QList<double> diffValues;
    double diffMoy = 0;
    for (int i = 0; i < fittsValues.size(); ++i) {
        diffValues.append(fabs(fittsValues[i] - this->fittsModel->times[i]));
        diffMoy += fabs(fittsValues[i] - this->fittsModel->times[i]);
    }
    diffMoy /= fittsValues.size();
    this->fittsModel->diffMoy = diffMoy;

    this->fittsModel->avgTime = 0;
    for (int i = 0; i < this->fittsModel->times.size(); ++i) {
        this->fittsModel->avgTime += this->fittsModel->times[i];
    }
    this->fittsModel->avgTime /= this->fittsModel->times.size();

    // Rafraîchir affichage des résultats
    this->displayResults();
}



void FittsController::addHisto() {
	this->histModel->prepend(*this->fittsModel);

	QDir().mkpath(
		QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));

	qDebug() << QStandardPaths::writableLocation(
		QStandardPaths::ConfigLocation);

	QString jsonPath =
		QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
		"/data.json";

	// std::string str = jsonPath.toStdString();
	// cout << "jsonPath " << str << endl;

	QFile fileReader(jsonPath);
	fileReader.open(QIODevice::ReadOnly);

	QJsonDocument json = QJsonDocument::fromJson(fileReader.readAll());
	QJsonArray array = json.array();
	array.prepend(this->fittsModel->writeDataJson());
	QJsonDocument newJson(array);

	fileReader.close();

	QFile fileWriter(jsonPath);
	fileWriter.open(QIODevice::WriteOnly);
	fileWriter.write(newJson.toJson());
	fileWriter.close();
}

QJsonArray FittsController::getHisto() {

	QString jsonPath =
		QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
		"/data.json";

	QFile fileReader(jsonPath);
	fileReader.open(QIODevice::ReadOnly);
	QJsonDocument json = QJsonDocument::fromJson(fileReader.readAll());
	QJsonArray array = json.array();

	fileReader.close();

	return array;
}

void FittsController::openJsonFile() {
	QDesktopServices::openUrl(
		QUrl(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
			 "/data.json"));
}

void FittsController::showHelp() {
	QMessageBox::information(this->fittsView, "Aide",
		"<h2>Qu'est-ce que la loi de Fitts ?</h2>"
		"Cette application permet d'experimenter la loi de Fitts. "
		"Basée sur une équation mathématique, "
		"la loi de Fitts est utilisée afin de mettre en évidence le temps "
		"nécessaire pour atteindre un objet cible. "
		"Quand on se met dans la cadre de l’IHM, un objet cible est n’importe "
		"quel élément interactif, comme un lien hypertexte, "
		"un bouton d’envoi ou un champ de saisie dans un formulaire sur "
		"internet."
		" Dans notre test les cibles seront des <span "
		"style='color:#D46679'>ronds rouges</span>."
		"<h2>Formule de la loi de Fitts</h2><br>"
		"<img src=':/img/formule'>"
		"<br>T représente le temps pour accomplir l'action, a et b sont des "
		"constantes empiriques, D est la distance de l'objet cible et L est la "
		"largeur de l'objet cible."
		"<h2>Déroulement du test</h2>"
		"Une fois sur la page de test il vous faudra appuyer sur l'écran. "
		"Ensuite, le test se lance "
		"avec les paramètres que vous avez saisis dans la page "
		"principale et dans la fenetre de dialogue des paramètres. "
		"Des <span style='color:#D46679'>cibles rouges</span>, sur lesquelles "
		"il faut cliquer le plus rapidement, apparaitront succesivement sur "
		"votre écran."
		"<h2>Résultats du test</h2>"
		"A la fin du test deux graphiques seront affichés. Le premier affiche "
		"le calcul de la loi de fitts pour chaque cible du test et  le second "
		"le temps d'exécution en fonction de la distance relative.");
}

void FittsController::displayResults() {
	this->fittsView->diffMoy->setText(
		"Différence moyenne = " + QString::number(this->fittsModel->diffMoy) +
		" ms");
    //this->fittsView->ecartType->setText(
    //	"Écart type = " + QString::number(this->fittsModel->ecartType) + " ms");
    //this->fittsView->erreurType->setText(
    //	"Erreur type = " + QString::number(this->fittsModel->erreurType) +
    //	" ms");
    //this->fittsView->itc95->setText("Intervalle de conf à 95% = " +
    //								QString::number(this->fittsModel->itc95) +
    //								" ms");
	this->fittsView->avgTime->setText(
		"Temps moyen = " + QString::number(this->fittsModel->avgTime) + " ms");
}

void FittsController::loadKeystrokeSettings() {
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->keystrokeSettingsScreenWidget);
	this->fittsView->inputK->setValue(this->keystrokeModel->k);
	this->fittsView->inputP->setValue(this->keystrokeModel->p);
	this->fittsView->inputH->setValue(this->keystrokeModel->h);
	this->fittsView->inputM->setValue(this->keystrokeModel->m);
}

void FittsController::saveKeystrokeSettings() {
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->keystrokeScreenWidget);
	this->keystrokeModel->k = this->fittsView->inputK->value();
	this->keystrokeModel->p = this->fittsView->inputP->value();
	this->keystrokeModel->h = this->fittsView->inputH->value();
	this->keystrokeModel->m = this->fittsView->inputM->value();
}

void FittsController::startKeystrokeEval() {
	qDebug() << "Démarrage éval Keystroke";
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->keystrokeEvalScreenWidget);
	QFileSystemModel *fileModel = new QFileSystemModel;
	fileModel->setRootPath(QDir::currentPath());
	this->fittsView->tree->setModel(fileModel);
	this->timer = new QElapsedTimer;
	timer->start();
}

void FittsController::endKeystrokeEval(const QModelIndex &index) {
	this->keystrokeModel->lastMeasuredTime = timer->elapsed();
	this->fittsView->mainStack->setCurrentWidget(
		this->fittsView->keystrokeResultScreenWidget);

	// Obtenir la profondeur de l'élément cliqué
	QModelIndex itIdx = index; // Copie de l'index pour pouvoir itérer dessus
	int depth = 0;
	while (itIdx.parent().isValid()) {
		itIdx = itIdx.parent();
		depth++;
	}

	int k = this->keystrokeModel->k;
	int p = this->keystrokeModel->p;
	int h = this->keystrokeModel->h;

	int theoreticalDuration = h + (p + k * 2) * depth + h;

	this->fittsView->keystrokeResultLabel->setText(
		"Temps réalisé : <span style='font-size: 32px'>" +
		QString::number(timer->elapsed()) + "</span> ms" +
		"<br>Temps théorique : <span style='font-size: 32px'>" +
		QString::number(theoreticalDuration) + "</span> ms" +
		"<br><br>Pour une profondeur de répertoires de " +
		QString::number(depth) +
		"<br><br>Note : la charge mentale n'est pas prise en compte dans ce "
	    "scénario car aucun fichier particulier n'est imposé." +
		"<br><br>Encodage Keystroke pour le calcul du temps théorique "
	    "(méthode souris) : H (PK)^(2*profondeur) H");
}
