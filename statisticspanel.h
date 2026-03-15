#ifndef STATISTICSPANEL_H
#define STATISTICSPANEL_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QVBoxLayout>

class BatteryData;

class StatisticsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsPanel(QWidget *parent = nullptr);
    ~StatisticsPanel();
    
    void setData(BatteryData *data);
    void showGroups(const QList<int> &groups);

private:
    void updateStats();
    
    BatteryData *m_data;
    QList<int> m_selectedGroups;
    QVBoxLayout *m_layout;
    QList<QLabel *> m_statLabels;
};

#endif // STATISTICSPANEL_H
