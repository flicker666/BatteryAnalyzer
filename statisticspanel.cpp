#include "statisticspanel.h"
#include "batterydata.h"
#include <QLabel>
#include <QGroupBox>
#include <QFont>

StatisticsPanel::StatisticsPanel(QWidget *parent)
    : QWidget(parent)
    , m_data(nullptr)
{
    setStyleSheet("background-color: #1e1e1e; border: 1px solid #3a3a3a; border-radius: 4px;");
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 10, 10, 10);
    
    QLabel *title = new QLabel("统计信息", this);
    title->setStyleSheet("font-size: 14px; font-weight: bold; color: #2a82da;");
    m_layout->addWidget(title);
    
    m_layout->addStretch();
    setLayout(m_layout);
}

StatisticsPanel::~StatisticsPanel()
{
}

void StatisticsPanel::setData(BatteryData *data)
{
    m_data = data;
    updateStats();
}

void StatisticsPanel::showGroups(const QList<int> &groups)
{
    m_selectedGroups = groups;
    updateStats();
}

void StatisticsPanel::updateStats()
{
    // Clear existing labels
    QLayoutItem *child;
    while ((child = m_layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    QLabel *title = new QLabel("统计信息", this);
    title->setStyleSheet("font-size: 14px; font-weight: bold; color: #2a82da;");
    m_layout->addWidget(title);
    
    if (!m_data || m_selectedGroups.isEmpty()) {
        QLabel *empty = new QLabel("请选择电池组", this);
        empty->setStyleSheet("color: #888;");
        m_layout->addWidget(empty);
        return;
    }
    
    // Calculate combined stats
    double minV = 1e9, maxV = -1e9, sum = 0;
    int count = 0;
    
    for (int g : m_selectedGroups) {
        BatteryData::GroupStats stats = m_data->groupStats(g);
        if (stats.min < minV) minV = stats.min;
        if (stats.max > maxV) maxV = stats.max;
        sum += stats.avg * BatteryData::CellsPerGroup * m_data->rowCount();
        count += BatteryData::CellsPerGroup * m_data->rowCount();
    }
    
    double avgV = sum / count;
    
    // Stat display
    QString style = "color: #ccc; font-size: 13px;";
    
    QLabel *rangeLabel = new QLabel(QString("电压范围: %1 - %2 mV").arg(minV, 0, 'f', 1).arg(maxV, 0, 'f', 1), this);
    rangeLabel->setStyleSheet(style);
    m_layout->addWidget(rangeLabel);
    
    QLabel *avgLabel = new QLabel(QString("平均电压: %1 mV").arg(avgV, 0, 'f', 1), this);
    avgLabel->setStyleSheet(style);
    m_layout->addWidget(avgLabel);
    
    QLabel *deltaLabel = new QLabel(QString("电压差值: %1 mV").arg(maxV - minV, 0, 'f', 1), this);
    deltaLabel->setStyleSheet(style);
    m_layout->addWidget(deltaLabel);
    
    m_layout->addSpacing(10);
    
    // Per-group stats
    for (int g : m_selectedGroups) {
        BatteryData::GroupStats stats = m_data->groupStats(g);
        
        QGroupBox *groupBox = new QGroupBox(QString("组 %1").arg(g + 1), this);
        groupBox->setStyleSheet("QGroupBox { color: #2a82da; font-weight: bold; border: 1px solid #3a3a3a; border-radius: 4px; margin-top: 8px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 5px; }");
        
        QVBoxLayout *groupLayout = new QVBoxLayout;
        groupLayout->addWidget(new QLabel(QString("最大: %1 mV").arg(stats.max, 0, 'f', 1), this));
        groupLayout->addWidget(new QLabel(QString("最小: %1 mV").arg(stats.min, 0, 'f', 1), this));
        groupLayout->addWidget(new QLabel(QString("平均: %1 mV").arg(stats.avg, 0, 'f', 1), this));
        groupLayout->addWidget(new QLabel(QString("标准差: %1").arg(stats.stdDev, 0, 'f', 2), this));
        
        groupBox->setLayout(groupLayout);
        m_layout->addWidget(groupBox);
    }
    
    m_layout->addStretch();
}
