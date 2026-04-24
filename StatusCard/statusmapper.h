#pragma once

#include <QString>
#include <QVariant>
#include <QHash>
#include <QVector>

enum class StatusLevel { Normal = 0, Warning = 1, Error = 2, Unknown = 3 };

class StatusMapper
{
public:
    struct RangeRule {
        int min = 0;
        int max = 0;
        StatusLevel level = StatusLevel::Unknown;
    };
    struct ValueRule {
        QVariant value;
        StatusLevel level = StatusLevel::Unknown;
    };

    StatusMapper() = default;

    void addRangeRule(const QString& field, int min, int max, StatusLevel level);
    void addValueRule(const QString& field, const QVariant& value, StatusLevel level);

    StatusLevel evaluate(const QString& field, const QVariant& value) const;
    bool validateConfig() const;

private:
    QHash<QString, QVector<RangeRule>> m_rangeRules;
    QHash<QString, QVector<ValueRule>> m_valueRules;
};
