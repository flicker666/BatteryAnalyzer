#include "heatmapwidget.h"
#include "batterydata.h"
#include <QPainter>
#include <QLabel>

HeatmapWidget::HeatmapWidget(QWidget *parent)
    : QWidget(parent)
    , m_data(nullptr)
    , m_currentRow(0)
    , m_hoveredGroup(-1)
    , m_hoveredCell(-1)
{
    setMouseTracking(true);
    setStyleSheet("background-color: #1e1e1e; border: 1px solid #3a3a3a; border-radius: 4px;");
}

HeatmapWidget::~HeatmapWidget()
{
}

void HeatmapWidget::setData(BatteryData *data)
{
    m_data = data;
    m_currentRow = 0;
    update();
}

QColor HeatmapWidget::getVoltageColor(double voltage)
{
    if (!m_data) return Qt::gray;
    
    double minV = m_data->minVoltage();
    double maxV = m_data->maxVoltage();
    double range = maxV - minV;
    
    if (range <= 0) return Qt::gray;
    
    double ratio = (voltage - minV) / range;
    
    // Blue (low) -> Green (mid) -> Red (high)
    if (ratio < 0.5) {
        // Blue to Green
        int blue = int(255 * (1 - ratio * 2));
        int green = int(255 * ratio * 2);
        return QColor(0, green, blue);
    } else {
        // Green to Red
        int red = int(255 * ((ratio - 0.5) * 2));
        int green = int(255 * (1 - (ratio - 0.5) * 2));
        return QColor(red, green, 0);
    }
}

void HeatmapWidget::drawCell(QPainter &painter, int x, int y, int group, int cell, double voltage)
{
    QColor color = getVoltageColor(voltage);
    painter.fillRect(x, y, CellWidth - 1, CellHeight - 1, color);
    
    if (group == m_hoveredGroup && cell == m_hoveredCell) {
        painter.setPen(QColor(255, 255, 255));
        painter.drawRect(x - 1, y - 1, CellWidth + 1, CellHeight + 1);
    }
}

void HeatmapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), QColor(30, 30, 30));
    
    if (!m_data) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "请加载数据文件");
        return;
    }
    
    // Title
    painter.setPen(QColor(42, 130, 218));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(Margin, 20, QString("电池组电压分布 (时间点: %1)").arg(m_data->timeAt(m_currentRow)));
    
    // Calculate grid dimensions
    const int groupWidth = BatteryData::CellsPerGroup * CellWidth;
    const int totalWidth = BatteryData::GroupCount * groupWidth;
    const int startX = (width() - totalWidth) / 2;
    const int startY = 50;
    
    // Draw group labels
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 9));
    for (int g = 0; g < BatteryData::GroupCount; ++g) {
        int x = startX + g * groupWidth + groupWidth/2 - 15;
        painter.drawText(x, startY - 5, QString("组%1").arg(g + 1));
    }
    
    // Draw heatmap for each group
    for (int g = 0; g < BatteryData::GroupCount; ++g) {
        for (int c = 0; c < BatteryData::CellsPerGroup; ++c) {
            double voltage = m_data->voltage(m_currentRow, g, c);
            int x = startX + g * groupWidth + c * CellWidth;
            int y = startY + (c / 13) * CellHeight;
            drawCell(painter, x, y, g, c, voltage);
        }
    }
    
    // Legend
    int legendY = height() - 30;
    painter.setPen(Qt::white);
    painter.drawText(Margin, legendY, QString("电压: %1 mV").arg(m_data->minVoltage(), 0, 'f', 0));
    painter.drawText(width() - 100, legendY, QString("%1 mV").arg(m_data->maxVoltage(), 0, 'f', 0));
    
    // Color gradient bar
    int legendWidth = 200;
    int legendX = Margin + 80;
    double vRange = m_data->maxVoltage() - m_data->minVoltage();
    for (int i = 0; i < legendWidth; ++i) {
        double ratio = (double)i / legendWidth;
        double v = m_data->minVoltage() + ratio * vRange;
        painter.fillRect(legendX + i, legendY - 15, 1, 10, getVoltageColor(v));
    }
}

void HeatmapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_data) return;
    
    const int groupWidth = BatteryData::CellsPerGroup * CellWidth;
    const int totalWidth = BatteryData::GroupCount * groupWidth;
    const int startX = (width() - totalWidth) / 2;
    const int startY = 50;
    
    int x = event->x() - startX;
    int y = event->y() - startY;
    
    if (x < 0 || y < 0 || x >= totalWidth) {
        m_hoveredGroup = -1;
        m_hoveredCell = -1;
        update();
        return;
    }
    
    m_hoveredGroup = x / groupWidth;
    m_hoveredCell = (x % groupWidth) / CellWidth;
    
    if (m_hoveredGroup >= BatteryData::GroupCount) {
        m_hoveredGroup = -1;
        m_hoveredCell = -1;
    }
    
    if (m_hoveredGroup >= 0 && m_hoveredCell >= 0 && m_hoveredCell < BatteryData::CellsPerGroup) {
        double voltage = m_data->voltage(m_currentRow, m_hoveredGroup, m_hoveredCell);
        QToolTip::showText(event->globalPos(), 
            QString("组%1 电池%2\n电压: %3 mV").arg(m_hoveredGroup + 1).arg(m_hoveredCell + 1).arg(voltage, 0, 'f', 1), 
            this);
    }
    
    update();
}

void HeatmapWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // Could implement row selection here
}

void HeatmapWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hoveredGroup = -1;
    m_hoveredCell = -1;
    update();
}
