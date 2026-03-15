#ifndef BATTERYDATA_H
#define BATTERYDATA_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>

class BatteryData : public QObject
{
    Q_OBJECT

public:
    explicit BatteryData(QObject *parent = nullptr);
    ~BatteryData();
    
    bool loadFromCSV(const QString &filePath);
    
    // Data access
    int rowCount() const { return m_rowCount; }
    int columnCount() const { return m_columnCount; } // 416 voltage values
    
    // Group data (8 groups × 52 cells)
    static constexpr int GroupCount = 8;
    static constexpr int CellsPerGroup = 52;
    static constexpr int TotalCells = GroupCount * CellsPerGroup; // 416
    
    // Get voltage at specific position
    double voltage(int row, int group, int cell) const;
    
    // Get all voltages for a group at a specific row
    QVector<double> voltagesForGroup(int row, int group) const;
    
    // Statistics for a group across all time
    struct GroupStats {
        double min;
        double max;
        double avg;
        double stdDev;
        double delta; // max - min
    };
    
    GroupStats groupStats(int group) const;
    
    // Time access
    QString timeAt(int row) const { return m_times.value(row, ""); }
    
    // Error
    QString lastError() const { return m_error; }
    
    // Data range
    double minVoltage() const { return m_minVoltage; }
    double maxVoltage() const { return m_maxVoltage; }

private:
    void calculateStats();
    
    // Raw data: row -> group -> cell -> voltage
    QVector<QVector<QVector<double>>> m_data;
    QStringList m_times;
    
    int m_rowCount;
    int m_columnCount;
    QString m_error;
    
    double m_minVoltage;
    double m_maxVoltage;
    
    // Cache stats
    QVector<GroupStats> m_groupStats;
    bool m_statsValid;
};

#endif // BATTERYDATA_H
