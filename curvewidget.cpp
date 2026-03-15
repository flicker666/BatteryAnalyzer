#include "curvewidget.h"
#include "batterydata.h"
#include <QVBoxLayout>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QFont>

CurveWidget::CurveWidget(QWidget *parent)
    : QWidget(parent)
    , m_data(nullptr)
    , m_chart(nullptr)
    , m_chartView(nullptr)
    , m_axisX(nullptr)
    , m_axisY(nullptr)
{
    setStyleSheet("background-color: #1e1e1e; border: 1px solid #3a3a3a; border-radius: 4px;");
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    // Create chart
    m_chart = new QChart();
    m_chart->setTitle("电压曲线");
    m_chart->setTitleFont(QFont("Arial", 11, QFont::Bold));
    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    m_chart->setPlotAreaBackgroundBrush(QBrush(QColor(25, 25, 25)));
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setLabelColor(Qt::white);
    m_chart->legend()->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
    
    // Axes
    m_axisX = new QValueAxis();
    m_axisX->setTitleText("时间 (采样点)");
    m_axisX->setTitleBrush(QBrush(QColor(170, 170, 170)));
    m_axisX->setLabelsBrush(QBrush(QColor(170, 170, 170)));
    m_axisX->setGridLineColor(QColor(60, 60, 60));
    m_axisX->setLinePenColor(QColor(100, 100, 100));
    
    m_axisY = new QValueAxis();
    m_axisY->setTitleText("电压 (mV)");
    m_axisY->setTitleBrush(QBrush(QColor(170, 170, 170)));
    m_axisY->setLabelsBrush(QBrush(QColor(170, 170, 170)));
    m_axisY->setGridLineColor(QColor(60, 60, 60));
    m_axisY->setLinePenColor(QColor(100, 100, 100));
    
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    
    // Chart view
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    
    layout->addWidget(m_chartView);
    setLayout(layout);
}

CurveWidget::~CurveWidget()
{
}

void CurveWidget::setData(BatteryData *data)
{
    m_data = data;
    updateChart();
}

void CurveWidget::showGroups(const QList<int> &groups)
{
    m_selectedGroups = groups;
    updateChart();
}

void CurveWidget::updateChart()
{
    if (!m_data) return;
    
    // Remove all series
    foreach(QAbstractSeries *series, m_chart->series()) {
        m_chart->removeSeries(series);
    }
    
    // Colors for different groups
    static const QColor groupColors[] = {
        QColor(42, 130, 218),   // Blue
        QColor(46, 204, 113),   // Green
        QColor(241, 196, 15),   // Yellow
        QColor(231, 76, 60),    // Red
        QColor(155, 89, 182),   // Purple
        QColor(230, 126, 34),  // Orange
        QColor(52, 152, 219),  // Light Blue
        QColor(26, 188, 156)    // Teal
    };
    
    int rowCount = qMin(m_data->rowCount(), 5000); // Limit for performance
    
    // Add series for each selected group (show average)
    for (int i = 0; i < m_selectedGroups.size() && i < 8; ++i) {
        int group = m_selectedGroups[i];
        
        QLineSeries *series = new QLineSeries();
        series->setName(QString("组 %1").arg(group + 1));
        
        // Sample every N points to reduce data
        int step = qMax(1, rowCount / 1000);
        
        for (int row = 0; row < rowCount; row += step) {
            // Calculate average for this group at this time
            double sum = 0;
            int count = 0;
            for (int cell = 0; cell < BatteryData::CellsPerGroup; ++cell) {
                double v = m_data->voltage(row, group, cell);
                if (v > 0) {
                    sum += v;
                    count++;
                }
            }
            if (count > 0) {
                series->append(row, sum / count);
            }
        }
        
        QPen pen = series->pen();
        pen.setColor(groupColors[group % 8]);
        pen.setWidth(1);
        series->setPen(pen);
        
        m_chart->addSeries(series);
        m_chart->attachAxis(series, m_axisX);
        m_chart->attachAxis(series, m_axisY);
    }
    
    // Update axis ranges
    m_axisX->setRange(0, rowCount);
    m_axisY->setRange(m_data->minVoltage() - 10, m_data->maxVoltage() + 10);
    
    m_chart->update();
}

void CurveWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (m_chart) {
        m_chart->setPlotAreaBackgroundRect(QRectF(0, 0, width() - 20, height() - 50));
    }
}
