#include "batterydata.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtMath>

BatteryData::BatteryData(QObject *parent)
    : QObject(parent)
    , m_rowCount(0)
    , m_columnCount(0)
    , m_minVoltage(0)
    , m_maxVoltage(0)
    , m_statsValid(false)
{
}

BatteryData::~BatteryData()
{
}

bool BatteryData::loadFromCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = QString("无法打开文件: %1").arg(file.errorString());
        return false;
    }
    
    QTextStream in(&file);
    
    // Read header line
    if (in.atEnd()) {
        m_error = "文件为空";
        return false;
    }
    
    QString header = in.readLine();
    QStringList headers = header.split(',');
    m_columnCount = headers.size() - 2; // Remove time and timezone
    
    // Reserve data
    m_data.clear();
    m_times.clear();
    m_rowCount = 0;
    m_statsValid = false;
    
    // Read data rows
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        
        QStringList values = line.split(',');
        if (values.size() < 2) continue;
        
        // First column is time, second is timezone, rest are voltages
        QString time = values[0];
        m_times.append(time);
        
        QVector<QVector<double>> rowData;
        rowData.resize(GroupCount);
        
        int voltageIndex = 2; // Start from V1
        for (int group = 0; group < GroupCount; ++group) {
            rowData[group].resize(CellsPerGroup);
            for (int cell = 0; cell < CellsPerGroup; ++cell) {
                if (voltageIndex < values.size()) {
                    bool ok;
                    double voltage = values[voltageIndex].toDouble(&ok);
                    if (!ok) voltage = 0;
                    rowData[group][cell] = voltage;
                    
                    // Track min/max
                    if (m_rowCount == 0 && group == 0 && cell == 0) {
                        m_minVoltage = m_maxVoltage = voltage;
                    } else {
                        if (voltage < m_minVoltage && voltage > 0) m_minVoltage = voltage;
                        if (voltage > m_maxVoltage) m_maxVoltage = voltage;
                    }
                }
                voltageIndex++;
            }
        }
        
        m_data.append(rowData);
        m_rowCount++;
        
        // Progress for large files
        if (m_rowCount % 1000 == 0) {
            qDebug() << "Loaded" << m_rowCount << "rows...";
        }
    }
    
    file.close();
    
    qDebug() << "Loaded" << m_rowCount << "rows," << m_columnCount << "columns";
    qDebug() << "Voltage range:" << m_minVoltage << "-" << m_maxVoltage;
    
    calculateStats();
    
    return true;
}

double BatteryData::voltage(int row, int group, int cell) const
{
    if (row < 0 || row >= m_data.size()) return 0;
    if (group < 0 || group >= GroupCount) return 0;
    if (cell < 0 || cell >= CellsPerGroup) return 0;
    
    return m_data[row][group][cell];
}

QVector<double> BatteryData::voltagesForGroup(int row, int group) const
{
    QVector<double> result;
    if (row < 0 || row >= m_data.size()) return result;
    if (group < 0 || group >= GroupCount) return result;
    
    result = m_data[row][group];
    return result;
}

BatteryData::GroupStats BatteryData::groupStats(int group) const
{
    if (group < 0 || group >= GroupCount || !m_statsValid) {
        return {0, 0, 0, 0, 0};
    }
    return m_groupStats[group];
}

void BatteryData::calculateStats()
{
    m_groupStats.resize(GroupCount);
    
    for (int group = 0; group < GroupCount; ++group) {
        GroupStats stats;
        stats.min = 1e9;
        stats.max = -1e9;
        stats.avg = 0;
        stats.delta = 0;
        
        double sum = 0;
        double sumSq = 0;
        int count = 0;
        
        for (int row = 0; row < m_data.size(); ++row) {
            for (int cell = 0; cell < CellsPerGroup; ++cell) {
                double v = m_data[row][group][cell];
                if (v <= 0) continue;
                
                if (v < stats.min) stats.min = v;
                if (v > stats.max) stats.max = v;
                
                sum += v;
                sumSq += v * v;
                count++;
            }
        }
        
        if (count > 0) {
            stats.avg = sum / count;
            stats.stdDev = qSqrt(sumSq / count - stats.avg * stats.avg);
            stats.delta = stats.max - stats.min;
        }
        
        m_groupStats[group] = stats;
    }
    
    m_statsValid = true;
}
