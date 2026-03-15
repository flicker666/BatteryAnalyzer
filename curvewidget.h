#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QtCharts>

class BatteryData;

class CurveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurveWidget(QWidget *parent = nullptr);
    ~CurveWidget();
    
    void setData(BatteryData *data);
    void showGroups(const QList<int> &groups);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateChart();
    
    BatteryData *m_data;
    QList<int> m_selectedGroups;
    QChart *m_chart;
    QChartView *m_chartView;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
};

#endif // CURVEWIDGET_H
