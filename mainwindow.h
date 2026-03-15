#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

#include "batterydata.h"
#include "heatmapwidget.h"
#include "curvewidget.h"
#include "statisticspanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenFile();
    void onGroupSelected(int group);
    void updateStatusBar();

private:
    void setupUI();
    void setupToolbar();
    void loadCSV(const QString &filePath);

    // UI Components
    QToolBar *toolbar;
    QSplitter *mainSplitter;
    QSplitter *leftSplitter;
    
    // Left panel
    QWidget *leftPanel;
    QListWidget *groupList;
    
    // Center panel
    HeatmapWidget *heatmapWidget;
    CurveWidget *curveWidget;
    StatisticsPanel *statsPanel;
    
    // Status bar
    QLabel *statusLabel;
    QLabel *dataInfoLabel;
    
    // Data
    BatteryData *batteryData;
};

#endif // MAINWINDOW_H
