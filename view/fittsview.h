#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "graphicwidget.h"

#include <QChart>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineSeries>
#include <QMainWindow>
#include <QPolarChart>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedLayout>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts>
#include <QFont>

class FittsModel;
class FittsController;

class FittsView : public QMainWindow {
	Q_OBJECT
public:
	FittsView(FittsController *fittsController, FittsModel *fittsModel);
	~FittsView();

    /**
     * @brief Initialiser les widgets de la fenêtre principale
     */
	void initWindows();

private:
    /**
     * @brief Mettre à jour le texte d'information lors d'un clic sur une cible
     */
	void updateTestMsg();

    /**
     * @brief Afficher l'historique des résultats
     */
	void displayHisto();

    /**
     * @brief Modifier dynamiquement l'apparence de l'interface courante
     * @param selectedAction - Action correspondante au thème sélectionné
     */
    void changeTheme(QAction* selectedAction);

    /**
     * @brief Initialiser les palettes de couleurs de l'interface
     */
    void initPalette();

    // MVC
	FittsModel *fittsModel;
	FittsController *fittsController;

    // Actions menubar
    QAction *actionDarkTheme;
    QAction *actionLightTheme;

    QAction *actionTutorial;

    // Stacked layout et écrans
    QStackedLayout *mainStack;

    QWidget *homeScreenWidget;
    QWidget *evalScreenWidget;
    QWidget *settingsScreenWidget;
    QWidget *resultsScreenWidget;
    QWidget *keystrokeScreenWidget;
    QWidget *keystrokeSettingsScreenWidget;
    QWidget *keystrokeEvalScreenWidget;
    QWidget *keystrokeResultScreenWidget;

    // Écran principal
    QToolButton *buttonStartFitts;
    QToolButton *buttonStartKeystroke;

    // Boutons changement écrans
	QPushButton *startBtn;
    QPushButton *btnFittsSettings;
    QPushButton *btnBackFromResults;
    QPushButton *btnGoToJson;
    
    QPushButton *btnSaveKeystrokeSettings;
    QPushButton *btnKeystrokeSettings;
    QPushButton *defaultScenarioBtn;

    // Keystroke
    QTreeView *tree;
    QLabel *keystrokeResultLabel;

    // Inputs valeurs
	QDoubleSpinBox *aValue;
	QDoubleSpinBox *bValue;
	QSpinBox *nbCible;
	QSpinBox *minSize;
	QSpinBox *maxSize;
	
	QSpinBox *inputK;
	QSpinBox *inputP;
	QSpinBox *inputH;
	QSpinBox *inputM;

    // Zone test Fitts
	GraphicWidget *graphicView;
	QGraphicsScene *scene;

    // Labels stats Fitts
	QLabel *testLabel;
	QLabel *ecartType;
	QLabel *erreurType;
	QLabel *diffMoy;
	QLabel *itc95;
    QLabel *avgTime;

    // Graphiques résultats
    QChartView *plotHome; ///< Vue pour le graphique du temps par cible
    QChartView *plotHomeDistance; ///< Vue pour le graphique du temps en fonction de la distance

    // Palettes
    QPalette darkPalette; ///< Palette de couleurs pour le thème sombre

	friend FittsController;
};

// Couleurs

static const QColor color_blue = QColor(0, 218, 199);
static const QColor color_purple = QColor(189, 136, 247);
static const QColor color_red = QColor(212, 102, 121);
static const QColor color_white = QColor(255, 255, 255);
static const QColor color_grid = QColor(61, 63, 68);

#endif // MAINWINDOW_H
