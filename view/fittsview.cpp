#include "fittsview.h"

#include "fittscontroller.h"
#include "fittsmodel.h"
#include "graphicwidget.h"

#include <iostream>
#include <QScreen>
#include <QGuiApplication>

FittsView::FittsView(FittsController *fittsController, FittsModel *fittsModel)
	: QMainWindow(), fittsModel(fittsModel) {

	// Définition d'une taille de fenêtre par défaut. La fenêtre reste
	// redimensionnable à tout moment.
    //this->resize(640, 480);
    // Taille adaptative en fonction de l'écran (pour supporter zoom Windows 175%)
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect availableGeometry = screen->availableGeometry();

    // Calculer une taille proportionnelle (60% de l'écran disponible)
    int width = static_cast<int>(availableGeometry.width() * 0.6);
    int height = static_cast<int>(availableGeometry.height() * 0.6);

    // Limiter aux dimensions min/max raisonnables
    width = qBound(500, width, 1000);
    height = qBound(400, height, 800);

    this->resize(width, height);

    //this->initPalette();
	this->initWindows();

	// Connexion slots-signaux

	// Changement d'écran
	connect(buttonStartFitts, SIGNAL(clicked()), fittsController,
		SLOT(initEvalScreen()));
	connect(
		startBtn, SIGNAL(clicked()), fittsController, SLOT(initEvalScreen()));
	connect(btnBackFromResults, SIGNAL(clicked()), fittsController,
		SLOT(initEvalScreen()));
	connect(btnFittsSettings, SIGNAL(clicked()), fittsController,
		SLOT(initSettingsScreen()));

	connect(
		btnGoToJson, SIGNAL(clicked()), fittsController, SLOT(openJsonFile()));
	connect(
		actionTutorial, SIGNAL(triggered()), fittsController, SLOT(showHelp()));

	connect(btnSaveKeystrokeSettings, SIGNAL(clicked()), fittsController,
		SLOT(saveKeystrokeSettings()));
	connect(btnKeystrokeSettings, SIGNAL(clicked()), fittsController,
		SLOT(loadKeystrokeSettings()));
	connect(defaultScenarioBtn, SIGNAL(clicked()), fittsController,
		SLOT(startKeystrokeEval()));
	connect(tree, SIGNAL(doubleClicked(const QModelIndex &)), fittsController,
		SLOT(endKeystrokeEval(const QModelIndex &)));

	// Clic sur une cible
	connect(graphicView, SIGNAL(mouseClicked(int, int)), fittsController,
		SLOT(cibleClicked(int, int)));

	// Changement de valeurs
	connect(aValue, SIGNAL(valueChanged(double)), fittsController,
		SLOT(aValueChanged(double)));
	connect(bValue, SIGNAL(valueChanged(double)), fittsController,
		SLOT(bValueChanged(double)));
	connect(nbCible, SIGNAL(valueChanged(int)), fittsController,
		SLOT(nbCibleChanged(int)));
	connect(minSize, SIGNAL(valueChanged(int)), fittsController,
		SLOT(minSizeChanged(int)));
	connect(maxSize, SIGNAL(valueChanged(int)), fittsController,
		SLOT(maxSizeChanged(int)));
}

FittsView::~FittsView() {}

void FittsView::initWindows() {

	// Appliquer le style depuis le fichier QSS
    QFile styleFile(":/style/global");
	styleFile.open(QFile::ReadOnly);
	QString styleString(styleFile.readAll());
    this->setStyleSheet(styleString);

	// Barre de menus

	QMenuBar *menuBar = new QMenuBar(this);

	QMenu *menuFile = new QMenu("&Fichier");

	QAction *actionQuitter = new QAction("&Quitter");
	actionQuitter->setIcon(QIcon(":/icons/close"));
	actionQuitter->setShortcut(tr("Ctrl+Q"));
	connect(actionQuitter, &QAction::triggered, qApp, &QApplication::quit);
	menuFile->addAction(actionQuitter);

	QMenu *menuView = new QMenu("Aff&ichage");
	QMenu *menuTheme = new QMenu("&Thème de l'interface");
	menuTheme->setIcon(QIcon(":/icons/palette"));

	actionDarkTheme = new QAction("Thème &sombre");
	actionDarkTheme->setCheckable(true);
	actionDarkTheme->setChecked(true);
	actionLightTheme = new QAction("Thème &clair");
	actionLightTheme->setCheckable(true);

	menuTheme->addAction(actionDarkTheme);
	menuTheme->addAction(actionLightTheme);

    QActionGroup *actionGroupTheme = new QActionGroup(this);
    actionGroupTheme->setExclusive(true);
    actionGroupTheme->addAction(actionDarkTheme);
    actionGroupTheme->addAction(actionLightTheme);
    connect(actionGroupTheme, &QActionGroup::triggered, this,
            &FittsView::changeTheme);


	menuView->addMenu(menuTheme);

/*	// Groupe d'actions pour modifier l'apparence
	QActionGroup *actionGroupTheme = new QActionGroup(this);
	actionGroupTheme->setExclusive(true);
	actionGroupTheme->addAction(actionDarkTheme);
	actionGroupTheme->addAction(actionLightTheme);
	connect(actionGroupTheme, &QActionGroup::triggered, this,
		&FittsView::changeTheme);
*/
	// Définir le thème sombre par défaut
	changeTheme(actionDarkTheme);

	QMenu *menuHelp = new QMenu("&?");

	actionTutorial = new QAction("&Aide");
	actionTutorial->setShortcut(Qt::Key_F1);
	menuHelp->addAction(actionTutorial);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	this->setMenuBar(menuBar);

	// Stack contenant tous les écrans

	QWidget *stackContainerWidget = new QWidget;
	this->setCentralWidget(stackContainerWidget);

	mainStack = new QStackedLayout(stackContainerWidget);
    //mainStack->setMargin(0);

	// Ecran d'accueil

	homeScreenWidget = new QWidget;
	QBoxLayout *homeScreenLayout = new QVBoxLayout;

	mainStack->addWidget(homeScreenWidget);

	QLayout *mainScreenAppChooserLayout = new QHBoxLayout;
	homeScreenWidget->setLayout(homeScreenLayout);

    buttonStartFitts = new QToolButton;  //elements de choix des la premiere page
	buttonStartFitts->setText("Loi de Fitts");
	buttonStartFitts->setCursor(QCursor(Qt::PointingHandCursor));
	buttonStartFitts->setIcon(QIcon(":/img/fitts"));
	buttonStartFitts->setIconSize(QSize(128, 128));
	buttonStartFitts->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonStartFitts->setToolTip("Lancer l'expérience de pointage basée sur la loi de Fitts.");

	buttonStartKeystroke = new QToolButton;
	buttonStartKeystroke->setText("Keystroke");
	buttonStartKeystroke->setCursor(QCursor(Qt::PointingHandCursor));
	buttonStartKeystroke->setIcon(QIcon(":/img/keystroke"));
	buttonStartKeystroke->setIconSize(QSize(128, 128));
	buttonStartKeystroke->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    buttonStartKeystroke->setToolTip("Accéder à l'expérience Keystroke pour évaluer des actions clavier/souris.");


    mainScreenAppChooserLayout->addWidget(buttonStartFitts);// ajout des boutons a la page principale
	mainScreenAppChooserLayout->addWidget(buttonStartKeystroke);

	QLabel *appChooserTitleLabel = new QLabel("Sélectionnez une application");
	appChooserTitleLabel->setProperty("class", "screen-title");

	homeScreenLayout->addWidget(appChooserTitleLabel);
	homeScreenLayout->addStretch();
	homeScreenLayout->addLayout(mainScreenAppChooserLayout);
	homeScreenLayout->addStretch();

	// Layouts

	settingsScreenWidget = new QWidget;
	QBoxLayout *settingsLayout = new QVBoxLayout(settingsScreenWidget);
	mainStack->addWidget(settingsScreenWidget);

	resultsScreenWidget = new QWidget;
	QBoxLayout *resultsLayout = new QVBoxLayout(resultsScreenWidget);
	mainStack->addWidget(resultsScreenWidget);

	QLabel *labelResultTitle = new QLabel("Résultats du test");
	labelResultTitle->setProperty("class", "screen-title");
	resultsLayout->addWidget(labelResultTitle);

	QBoxLayout *settingsSubLayout = new QVBoxLayout(settingsScreenWidget);
	QBoxLayout *settingsLayoutRight = new QVBoxLayout(settingsScreenWidget);

	QFrame *frameRight = new QFrame();
	frameRight->setLayout(settingsLayoutRight);

	QLabel *labelSettingsTitle = new QLabel("Paramètres");
	labelSettingsTitle->setProperty("class", "screen-title");

	settingsLayout->addWidget(labelSettingsTitle);
	settingsLayout->addLayout(settingsSubLayout);

	QLabel *label;

	// Layout résultat: stats
	QWidget *statsWidget = new QWidget();
    statsWidget->setMaximumHeight(280);//********************

	resultsLayout->addWidget(statsWidget);

    QHBoxLayout *globalResultLayout = new QHBoxLayout();//***********************************************
    statsWidget->setLayout(globalResultLayout);//********************************************************

    QVBoxLayout *statsWidgetLayout = new QVBoxLayout();
    globalResultLayout->addLayout(statsWidgetLayout);//**************************************************
    //statsWidget->setLayout(statsWidgetLayout);

    //ecartType = new QLabel();
    //statsWidgetLayout->addWidget(ecartType);

	diffMoy = new QLabel();
	statsWidgetLayout->addWidget(diffMoy);
    diffMoy->setToolTip("Différence moyenne entre le modèle et l'expérience.");


    //erreurType = new QLabel();
    //statsWidgetLayout->addWidget(erreurType);

    //itc95 = new QLabel();
    //statsWidgetLayout->addWidget(itc95);

	avgTime = new QLabel();
	statsWidgetLayout->addWidget(avgTime);
    avgTime->setToolTip("Temps moyen observé pour atteindre les cibles.");


    // Partie gauche : paramètres a et b//************************************************************************/
   /* QBoxLayout *globalResultLayout = new QHBoxLayout();
    statsWidget->setLayout(globalResultLayout);


    QWidget *paramsWidget = new QWidget();
    QVBoxLayout *paramsLayout = new QVBoxLayout(paramsWidget);

    QLabel *labelA = new QLabel("Paramètre a :");
    aValue = new QDoubleSpinBox();
    aValue->setRange(0.0, 1.0); // plage possible pour a
    aValue->setSingleStep(0.01);
    aValue->setDecimals(4);
    paramsLayout->addWidget(labelA);
    paramsLayout->addWidget(aValue);

    QLabel *labelB = new QLabel("Paramètre b :");
    bValue = new QDoubleSpinBox();
    bValue->setRange(0.0, 1.0);
    bValue->setSingleStep(0.01);
    bValue->setDecimals(4);
    paramsLayout->addWidget(labelB);
    paramsLayout->addWidget(bValue);

    globalResultLayout->addWidget(paramsWidget);*******************************************************************/

    QVBoxLayout *paramsLayout = new QVBoxLayout();//***************************************
    globalResultLayout->addLayout(paramsLayout);//*****************************************



    {
        /*QBoxLayout *settingLayout = new QVBoxLayout;//**************************************

        label = new QLabel();
        label->setText("Variable A");
        label->setAlignment(Qt::AlignCenter);
        settingLayout->addWidget(label);*/

        // Paramètre A
        QLabel *labelA = new QLabel("Paramètre a :");//****************************************
        paramsLayout->addWidget(labelA);


        aValue = new QDoubleSpinBox;
        aValue->setValue(this->fittsModel->a);
        aValue->setMinimum(0.1);
        aValue->setSingleStep(0.1);
        aValue->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        aValue->setCursor(Qt::PointingHandCursor);
        paramsLayout->addWidget(aValue);
        aValue->setToolTip("Coefficient a : ajustez cette valeur pour observer son influence sur la courbe théorique.");


        //statsWidgetLayout->addLayout(settingLayout);
    }

    {
        /*QBoxLayout *settingLayout = new QVBoxLayout;//******************************************************

        label = new QLabel();
        label->setText("Variable B");
        label->setAlignment(Qt::AlignCenter);
        settingLayout->addWidget(label);*/

        // Paramètre B
        QLabel *labelB = new QLabel("Paramètre b :");//*****************************************
        paramsLayout->addWidget(labelB);

        bValue = new QDoubleSpinBox;
        bValue->setValue(this->fittsModel->b);
        bValue->setMinimum(0.1);
        bValue->setSingleStep(0.1);
        bValue->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        bValue->setCursor(Qt::PointingHandCursor);
        paramsLayout->addWidget(bValue);
        bValue->setToolTip("Coefficient b : ajustez cette valeur pour observer son influence sur la courbe théorique.");

       // statsWidgetLayout->addLayout(settingLayout);
    }


	QBoxLayout *graphHomeLayout = new QVBoxLayout();
	resultsLayout->addLayout(graphHomeLayout);

	// Légende graphiques
	QBoxLayout *titleLegend = new QHBoxLayout();

	titleLegend->addStretch();

	QLabel *legendTitle = new QLabel(this);
	legendTitle->setText("Légende : ");
	titleLegend->addWidget(legendTitle);

	titleLegend->addSpacing(8);

	QLabel *legendTheo = new QLabel(this);
	legendTheo->setText("Théorique");
	legendTheo->setProperty("class", "theorique");
	titleLegend->addWidget(legendTheo);

	titleLegend->addSpacing(8);

	QLabel *legendExp = new QLabel(this);
	legendExp->setText("Expérimental");
	legendExp->setProperty("class", "exp");
	titleLegend->addWidget(legendExp);

	titleLegend->addSpacing(12);

	graphHomeLayout->addLayout(titleLegend);

	// Graphiques

	plotHome = new QChartView;
	plotHome->setRenderHint(QPainter::Antialiasing);
	plotHome->setRubberBand(QChartView::RectangleRubberBand);

	plotHomeDistance = new QChartView;
	plotHomeDistance->setRenderHint(QPainter::Antialiasing);
	plotHomeDistance->setRubberBand(QChartView::RectangleRubberBand);

	QTabWidget *graphSelector = new QTabWidget;
	graphSelector->addTab(plotHome, "Temps par cible");
	graphSelector->addTab(plotHomeDistance, "Temps en fonction de la distance");

    graphHomeLayout->addWidget(graphSelector);
    graphSelector->setMinimumHeight(200);

	QBoxLayout *bottomActionsResultLayout = new QHBoxLayout;
	resultsLayout->addLayout(bottomActionsResultLayout);

	btnBackFromResults = new QPushButton("Revenir au test");
	btnBackFromResults->setIcon(QIcon(":/icons/arrow_back"));
	btnBackFromResults->setIconSize(QSize(24, 24));
	btnBackFromResults->setCursor(Qt::PointingHandCursor);
	bottomActionsResultLayout->addWidget(btnBackFromResults);

	btnGoToJson = new QPushButton("Voir le JSON");
	btnGoToJson->setIcon(QIcon(":/icons/source"));
	btnGoToJson->setIconSize(QSize(24, 24));
	btnGoToJson->setCursor(Qt::PointingHandCursor);
	bottomActionsResultLayout->addWidget(btnGoToJson);

	// Layout paramètres

	QFrame *cardBottom = new QFrame();
	settingsSubLayout->addWidget(cardBottom);

	QGroupBox *variableSettingsFrame = new QGroupBox("Paramètres de formule");
	QBoxLayout *variableSettingsLayout = new QHBoxLayout(variableSettingsFrame);
	settingsSubLayout->addWidget(variableSettingsFrame);

	label = new QLabel();
	label->setText("a + b*log2(2D/L)");
	label->setPixmap(QPixmap(":/img/formule"));
	label->setAlignment(Qt::AlignCenter);
	variableSettingsLayout->addWidget(label);

    /*{
        QBoxLayout *settingLayout = new QVBoxLayout;//******************************************

		label = new QLabel();
		label->setText("Variable A");
		label->setAlignment(Qt::AlignCenter);
		settingLayout->addWidget(label);

		aValue = new QDoubleSpinBox;
		aValue->setValue(this->fittsModel->a);
		aValue->setMinimum(0.1);
		aValue->setSingleStep(0.1);
		aValue->setButtonSymbols(QAbstractSpinBox::PlusMinus);
		aValue->setCursor(Qt::PointingHandCursor);
		settingLayout->addWidget(aValue);

		variableSettingsLayout->addLayout(settingLayout);
    }*/

    /*{
        QBoxLayout *settingLayout = new QVBoxLayout;//*****************************************************

		label = new QLabel();
		label->setText("Variable B");
		label->setAlignment(Qt::AlignCenter);
		settingLayout->addWidget(label);

		bValue = new QDoubleSpinBox;
		bValue->setValue(this->fittsModel->b);
		bValue->setMinimum(0.1);
		bValue->setSingleStep(0.1);
		bValue->setButtonSymbols(QAbstractSpinBox::PlusMinus);
		bValue->setCursor(Qt::PointingHandCursor);
		settingLayout->addWidget(bValue);

		variableSettingsLayout->addLayout(settingLayout);
    }*/

    QGroupBox *evalSettingsFrame = new QGroupBox("Paramètres du test");//********************************************
	QBoxLayout *configLayout = new QHBoxLayout(evalSettingsFrame);
	settingsSubLayout->addWidget(evalSettingsFrame);

	QBoxLayout *configLayoutItem = new QVBoxLayout();
    /*label = new QLabel();
	label->setText("Nombre de cibles");
	label->setAlignment(Qt::AlignCenter);
    configLayoutItem->addWidget(label);*/

    QLabel *labelNbCible = new QLabel("Nombre de cibles :");
    paramsLayout->addWidget(labelNbCible);

	nbCible = new QSpinBox;
	nbCible->setValue(this->fittsModel->nbCible);
    nbCible->setMaximum(100);
    nbCible->setMinimum(5);
	nbCible->setSingleStep(1);
	nbCible->setCursor(Qt::PointingHandCursor);
    paramsLayout->addWidget(nbCible);
    //configLayout->addLayout(configLayoutItem);
    statsWidgetLayout->addLayout(configLayoutItem);//*****************************************************
    nbCible->setToolTip("Nombre de cibles pendant le test. Recommandé : entre 5 et 10.");

	QFrame *separator = new QFrame;
	separator->setMinimumWidth(2);
	separator->setMaximumWidth(2);
	configLayout->addWidget(separator);

	configLayoutItem = new QVBoxLayout();
	label = new QLabel();
	label->setText("Taille minimum");
	label->setAlignment(Qt::AlignCenter);
	configLayoutItem->addWidget(label);

	minSize = new QSpinBox;
	minSize->setMaximum(150);
	minSize->setMinimum(10);
	minSize->setSingleStep(10);
	minSize->setValue(this->fittsModel->minSize);
	minSize->setCursor(Qt::PointingHandCursor);
	configLayoutItem->addWidget(minSize);
	configLayout->addLayout(configLayoutItem);

	configLayoutItem = new QVBoxLayout();
	label = new QLabel();
	label->setText("Taille maximum");
	label->setAlignment(Qt::AlignCenter);
	configLayoutItem->addWidget(label);

	maxSize = new QSpinBox;
	maxSize->setMaximum(400);
	maxSize->setMinimum(160);
	maxSize->setSingleStep(10);
	maxSize->setValue(this->fittsModel->maxSize);
	maxSize->setCursor(Qt::PointingHandCursor);
	configLayoutItem->addWidget(maxSize);
	configLayout->addLayout(configLayoutItem);

	settingsSubLayout->addStretch();

	startBtn = new QPushButton("Enregistrer les paramètres");
	startBtn->setIcon(QIcon(":/icons/save"));
	startBtn->setIconSize(QSize(24, 24));
	startBtn->setCursor(Qt::PointingHandCursor);
	settingsSubLayout->addWidget(startBtn);

	// Ecran de test

	evalScreenWidget = new QWidget;
	mainStack->addWidget(evalScreenWidget);

	QBoxLayout *testLayout = new QVBoxLayout(evalScreenWidget);

	graphicView = new GraphicWidget;
	scene = new QGraphicsScene;
	graphicView->setScene(scene);
	graphicView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	scene->setSceneRect(QRect(0, 0, graphicView->width() - 64, 256));

	QBoxLayout *bottomLayout = new QHBoxLayout;

	btnFittsSettings = new QPushButton("Réglages du test");
	btnFittsSettings->setIcon(QIcon(":/icons/settings"));
	btnFittsSettings->setIconSize(QSize(24, 24));
	btnFittsSettings->setCursor(Qt::PointingHandCursor);

	QPushButton *btnBackToMenu = new QPushButton("Retour");
	btnBackToMenu->setIcon(QIcon(":/icons/arrow_back"));
	btnBackToMenu->setIconSize(QSize(24, 24));
	btnBackToMenu->setCursor(Qt::PointingHandCursor);
	connect(btnBackToMenu, &QPushButton::clicked, this,
		[this]() { mainStack->setCurrentWidget(homeScreenWidget); });

	testLabel = new QLabel;
	testLabel->setText("");
	testLabel->setAlignment(Qt::AlignCenter);

	bottomLayout->addWidget(btnBackToMenu);
	bottomLayout->addWidget(btnFittsSettings);

	testLayout->addWidget(testLabel);
	testLayout->addWidget(graphicView);
	testLayout->addLayout(bottomLayout);

	// Ecran keystroke
	keystrokeScreenWidget = new QWidget;
	QBoxLayout *keystrokeLayout = new QVBoxLayout;
	keystrokeScreenWidget->setLayout(keystrokeLayout);

	mainStack->addWidget(keystrokeScreenWidget);
	connect(buttonStartKeystroke, &QPushButton::clicked, this,
		[this]() { mainStack->setCurrentWidget(keystrokeScreenWidget); });

	QLabel *keystrokeHomeTitle = new QLabel("Liste des scénarios Keystroke");
	keystrokeHomeTitle->setProperty("class", "screen-title");
	keystrokeLayout->addWidget(keystrokeHomeTitle);

	QWidget *scenariosWidget = new QGroupBox("Scénarios préinstallés");
	QBoxLayout *scenariosLayout = new QVBoxLayout;
	scenariosWidget->setLayout(scenariosLayout);
	keystrokeLayout->addWidget(scenariosWidget);

	QWidget *defaultScenarioItem = new QWidget;
	QBoxLayout *defaultScenarioItemLayout = new QVBoxLayout;
	defaultScenarioItem->setLayout(defaultScenarioItemLayout);
	defaultScenarioItem->setProperty("class", "scenario-container");
	scenariosLayout->addWidget(defaultScenarioItem);

	QLabel *defaultScenarioTitle =
		new QLabel("Exemple : Ouvrir un fichier dans une arborescence");
	defaultScenarioTitle->setProperty("class", "subtitle");
	defaultScenarioItemLayout->addWidget(defaultScenarioTitle);

	QLabel *keystrokeEvalGuideLabel = new QLabel(
		"Ce test contient une arborescence de fichiers basée sur votre système "
		"de fichiers local. Double-cliquez sur un élément pour compléter le "
		"test et comparer votre résultat avec le temps théorique.");
	keystrokeEvalGuideLabel->setWordWrap(true);
	defaultScenarioItemLayout->addWidget(keystrokeEvalGuideLabel);

    keystrokeEvalGuideLabel->setObjectName("keystrokeGuide");


	QBoxLayout *defaultScenarioItemActionsLayout = new QHBoxLayout;
	defaultScenarioItemLayout->addLayout(defaultScenarioItemActionsLayout);

	defaultScenarioBtn = new QPushButton("Démarrer");
	defaultScenarioBtn->setCursor(Qt::PointingHandCursor);
	defaultScenarioItemActionsLayout->addStretch();
	defaultScenarioItemActionsLayout->addWidget(defaultScenarioBtn);

	scenariosLayout->addStretch();

	QBoxLayout *keystrokeBottomBarLayout = new QHBoxLayout;
	keystrokeLayout->addLayout(keystrokeBottomBarLayout);

	QPushButton *btnBackToMenuFromKeystroke = new QPushButton("Retour");
	btnBackToMenuFromKeystroke->setIcon(QIcon(":/icons/arrow_back"));
	btnBackToMenuFromKeystroke->setIconSize(QSize(24, 24));
	btnBackToMenuFromKeystroke->setCursor(Qt::PointingHandCursor);
	connect(btnBackToMenuFromKeystroke, &QPushButton::clicked, this,
		[this]() { mainStack->setCurrentWidget(homeScreenWidget); });
	keystrokeBottomBarLayout->addWidget(btnBackToMenuFromKeystroke);

	btnKeystrokeSettings = new QPushButton("Paramètres des durées");
	btnKeystrokeSettings->setIcon(QIcon(":/icons/settings"));
	btnKeystrokeSettings->setIconSize(QSize(24, 24));
	btnKeystrokeSettings->setCursor(Qt::PointingHandCursor);
	keystrokeBottomBarLayout->addWidget(btnKeystrokeSettings);

	// Ecran paramètres keystroke
	keystrokeSettingsScreenWidget = new QWidget;
	QBoxLayout *keystrokeSettingsLayout = new QVBoxLayout;
	keystrokeSettingsScreenWidget->setLayout(keystrokeSettingsLayout);
	mainStack->addWidget(keystrokeSettingsScreenWidget);

	QLabel *keystrokeSettingsTitle =
		new QLabel("Paramètres des durées théoriques");
	keystrokeSettingsTitle->setProperty("class", "screen-title");
	keystrokeSettingsLayout->addWidget(keystrokeSettingsTitle);

	inputK = new QSpinBox;
	inputK->setMaximum(10000);
	inputK->setMinimum(0);
	inputK->setSingleStep(100);

	inputP = new QSpinBox;
	inputP->setMaximum(10000);
	inputP->setMinimum(0);
	inputP->setSingleStep(100);

	inputH = new QSpinBox;
	inputH->setMaximum(10000);
	inputH->setMinimum(0);
	inputH->setSingleStep(100);

	inputM = new QSpinBox;
	inputM->setMaximum(10000);
	inputM->setMinimum(0);
	inputM->setSingleStep(100);

	QLabel *labelK = new QLabel("K : Frappe/clic (en ms) : ");
	labelK->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLabel *labelP = new QLabel("P : Déplacement de souris (en ms) : ");
	labelP->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLabel *labelH = new QLabel("H : Changement souris-clavier (en ms) : ");
	labelH->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QLabel *labelM = new QLabel("M : Réflexion mentale (en ms) : ");
	labelM->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QFormLayout *keystrokeParamsLayout = new QFormLayout();
	keystrokeParamsLayout->addRow(labelK, inputK);
	keystrokeParamsLayout->addRow(labelP, inputP);
	keystrokeParamsLayout->addRow(labelH, inputH);
	keystrokeParamsLayout->addRow(labelM, inputM);
	keystrokeSettingsLayout->addLayout(keystrokeParamsLayout);

	keystrokeSettingsLayout->addStretch();

	btnSaveKeystrokeSettings = new QPushButton("Enregistrer les paramètres");
	btnSaveKeystrokeSettings->setIcon(QIcon(":/icons/save"));
	btnSaveKeystrokeSettings->setIconSize(QSize(24, 24));
	btnSaveKeystrokeSettings->setCursor(Qt::PointingHandCursor);
	keystrokeSettingsLayout->addWidget(btnSaveKeystrokeSettings);

	// Ecran évaluation Keystroke
	keystrokeEvalScreenWidget = new QWidget;
	QBoxLayout *keystrokeEvalLayout = new QVBoxLayout;
	keystrokeEvalScreenWidget->setLayout(keystrokeEvalLayout);
	mainStack->addWidget(keystrokeEvalScreenWidget);
	tree = new QTreeView;
	tree->resizeColumnToContents(0);
	keystrokeEvalLayout->addWidget(tree);

	QPushButton *btnBackFromKeystrokeEval =
		new QPushButton("Interrompre le test");
	btnBackFromKeystrokeEval->setIcon(QIcon(":/icons/close"));
	btnBackFromKeystrokeEval->setIconSize(QSize(24, 24));
	btnBackFromKeystrokeEval->setCursor(Qt::PointingHandCursor);
	connect(btnBackFromKeystrokeEval, &QPushButton::clicked, this,
		[this]() { mainStack->setCurrentWidget(keystrokeScreenWidget); });
	keystrokeEvalLayout->addWidget(btnBackFromKeystrokeEval);

	// Ecran résultats Keystroke
	keystrokeResultScreenWidget = new QWidget;
	QBoxLayout *keystrokeResultLayout = new QVBoxLayout;
	keystrokeResultScreenWidget->setLayout(keystrokeResultLayout);
	mainStack->addWidget(keystrokeResultScreenWidget);

	QLabel *keystrokeResultTitle = new QLabel("Résultats du test");
	keystrokeResultTitle->setProperty("class", "screen-title");
	keystrokeResultLayout->addWidget(keystrokeResultTitle);

	keystrokeResultLabel = new QLabel;
	keystrokeResultLayout->addWidget(keystrokeResultLabel);
	keystrokeResultLabel->setWordWrap(true);

	keystrokeResultLayout->addStretch();

	QPushButton *btnBackFromKeystrokeResult = new QPushButton("Retour");
	btnBackFromKeystrokeResult->setIcon(QIcon(":/icons/arrow_back"));
	btnBackFromKeystrokeResult->setIconSize(QSize(24, 24));
	btnBackFromKeystrokeResult->setCursor(Qt::PointingHandCursor);
	connect(btnBackFromKeystrokeResult, &QPushButton::clicked, this,
		[this]() { mainStack->setCurrentWidget(keystrokeScreenWidget); });
	keystrokeResultLayout->addWidget(btnBackFromKeystrokeResult);
}

/*void FittsView::initPalette() {
	darkPalette.setColor(QPalette::Window, QColor(28, 29, 31));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(
		QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
	darkPalette.setColor(QPalette::Base, QColor(39, 40, 43));
	darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, QColor(28, 29, 31));
	darkPalette.setColor(
		QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
	darkPalette.setColor(QPalette::Dark, QColor(16, 17, 18));
	darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
	darkPalette.setColor(QPalette::Button, QColor(38, 39, 41));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(
		QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::Highlight, QColor(63, 145, 232));
	darkPalette.setColor(
		QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
	darkPalette.setColor(QPalette::HighlightedText, Qt::white);
	darkPalette.setColor(
		QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
}*/

void FittsView::updateTestMsg() {
	this->testLabel->setText(
		"<span style='font-size: 64px; font-weight: 500'>" +
		QString::number(this->fittsModel->cibleLeft) +
		"</span> cibles restantes");
}

void FittsView::changeTheme(QAction* selectedAction) {
    QFile styleFile;
    if (selectedAction == actionDarkTheme)
        styleFile.setFileName(":/style/dark.qss");
    else
        styleFile.setFileName(":/style/light.qss");

    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
    }
}



/*void FittsView::changeTheme(QAction *selectedAction) {
	if (selectedAction == actionDarkTheme)
		qApp->setPalette(darkPalette);
	if (selectedAction == actionLightTheme)
		qApp->setPalette(style()->standardPalette());

	qApp->setStyle(QStyleFactory::create("Fusion"));
}*/
