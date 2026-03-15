#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>

class BatteryData;

class HeatmapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HeatmapWidget(QWidget *parent = nullptr);
    ~HeatmapWidget();
    
    void setData(BatteryData *data);
    void setCurrentRow(int row) { m_currentRow = row; update(); }
    int currentRow() const { return m_currentRow; }

signals:
    void rowSelected(int row);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor getVoltageColor(double voltage);
    void drawCell(QPainter &painter, int x, int y, int group, int cell, double voltage);
    
    BatteryData *m_data;
    int m_currentRow;
    int m_hoveredGroup;
    int m_hoveredCell;
    
    static constexpr int CellWidth = 20;
    static constexpr int CellHeight = 14;
    static constexpr int Margin = 30;
};

#endif // HEATMAPWIDGET_H
