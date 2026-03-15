#include "mainwindow.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QApplication>
#include <QPalette>
#include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , batteryData(new BatteryData(this))
{
    setupUI();
    setupToolbar();
    
    setWindowTitle("电池数据分析系统");
    resize(1400, 900);
    setMinimumSize(1200, 700);
    
    // Dark theme
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(30, 30, 30));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base, QColor(45, 45, 45));
    pal.setColor(QPalette::AlternateBase, QColor(35, 35, 35));
    pal.setColor(QPalette::ToolTipBase, QColor(50, 50, 50));
    pal.setColor(QPalette::ToolTipText, Qt::white);
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(60, 60, 60));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(42, 130, 218));
    pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    setPalette(pal);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main splitter
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Left panel - Group selection
    leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel *groupLabel = new QLabel("电池组", this);
    groupLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2a82da;");
    leftLayout->addWidget(groupLabel);
    
    groupList = new QListWidget(this);
    groupList->setMinimumWidth(150);
    for (int i = 0; i < 8; ++i) {
        QListWidgetItem *item = new QListWidgetItem(QString("组 %1").arg(i + 1), groupList);
        item->setCheckState(Qt::Checked);
        item->setData(Qt::UserRole, i);
    }
    connect(groupList, &QListWidget::itemChanged, this, &MainWindow::onGroupSelected);
    leftLayout->addWidget(groupList);
    
    leftPanel->setLayout(leftLayout);
    mainSplitter->addWidget(leftPanel);
    
    // Right panel - Main content
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(8);
    
    // Heatmap
    heatmapWidget = new HeatmapWidget(this);
    heatmapWidget->setMinimumHeight(300);
    rightLayout->addWidget(heatmapWidget);
    
    // Curve and stats split
    QSplitter *bottomSplitter = new QSplitter(Qt::Horizontal, this);
    
    curveWidget = new CurveWidget(this);
    statsPanel = new StatisticsPanel(this);
    
    bottomSplitter->addWidget(curveWidget);
    bottomSplitter->addWidget(statsPanel);
    bottomSplitter->setSizes({800, 400});
    
    rightLayout->addWidget(bottomSplitter);
    rightPanel->setLayout(rightLayout);
    
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({200, 1200});
    
    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainSplitter);
    
    // Status bar
    statusBar()->setStyleSheet("QStatusBar { background: #1e1e1e; color: #aaa; }");
    statusLabel = new QLabel("就绪", this);
    dataInfoLabel = new QLabel("", this);
    statusBar()->addWidget(statusLabel);
    statusBar()->addPermanentWidget(dataInfoLabel);
}

void MainWindow::setupToolbar()
{
    toolbar = addToolBar("工具栏");
    toolbar->setMovable(false);
    toolbar->setStyleSheet("QToolBar { background: #252525; spacing: 10px; padding: 5px; }"
                          "QToolButton { background: #3a3a3a; color: white; padding: 5px 15px; border-radius: 3px; }"
                          "QToolButton:hover { background: #4a4a4a; }");
    
    QAction *openAction = new QAction("📂 打开文件", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenFile);
    toolbar->addAction(openAction);
    
    toolbar->addSeparator();
    
    QLabel *titleLabel = new QLabel("  电池数据分析系统  ");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2a82da;");
    toolbar->addWidget(titleLabel);
}

void MainWindow::onOpenFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "打开电池数据文件",
        "",
        "CSV Files (*.csv);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        loadCSV(filePath);
    }
}

void MainWindow::loadCSV(const QString &filePath)
{
    statusLabel->setText("加载中...");
    QApplication::processEvents();
    
    if (batteryData->loadFromCSV(filePath)) {
        heatmapWidget->setData(batteryData);
        curveWidget->setData(batteryData);
        statsPanel->setData(batteryData);
        
        // Select first group by default
        onGroupSelected(0);
        
        updateStatusBar();
        statusLabel->setText("文件加载成功");
    } else {
        QMessageBox::critical(this, "错误", "无法加载文件: " + batteryData->lastError());
        statusLabel->setText("加载失败");
    }
}

void MainWindow::onGroupSelected(int group)
{
    Q_UNUSED(group);
    
    // Get selected groups
    QList<int> selectedGroups;
    for (int i = 0; i < groupList->count(); ++i) {
        if (groupList->item(i)->checkState() == Qt::Checked) {
            selectedGroups.append(i);
        }
    }
    
    if (selectedGroups.isEmpty()) {
        statusLabel->setText("请至少选择一个电池组");
        return;
    }
    
    curveWidget->showGroups(selectedGroups);
    statsPanel->showGroups(selectedGroups);
}

void MainWindow::updateStatusBar()
{
    QString info = QString("数据点: %1 | 采样时间: %2 - %3")
        .arg(batteryData->rowCount())
        .arg(batteryData->timeAt(0))
        .arg(batteryData->timeAt(batteryData->rowCount() - 1));
    dataInfoLabel->setText(info);
}
