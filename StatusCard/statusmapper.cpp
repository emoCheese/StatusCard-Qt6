#include "statusmapper.h"
#include <QDebug>

void StatusMapper::addRangeRule(const QString& field, int min, int max, StatusLevel level)
{
    m_rangeRules[field].append({min, max, level});
}

void StatusMapper::addValueRule(const QString& field, const QVariant& value, StatusLevel level)
{
    m_valueRules[field].append({value, level});
}

StatusLevel StatusMapper::evaluate(const QString& field, const QVariant& value) const
{
    auto vit = m_valueRules.find(field);
    if (vit != m_valueRules.end()) {
        for (const auto& rule : vit.value()) {
            if (rule.value == value) {
                return rule.level;
            }
        }
    }

    auto rit = m_rangeRules.find(field);
    if (rit != m_rangeRules.end()) {
        bool ok = false;
        int intValue = value.toInt(&ok);
        if (ok) {
            for (const auto& rule : rit.value()) {
                if (intValue >= rule.min && intValue <= rule.max) {
                    return rule.level;
                }
            }
        }
    }

    return StatusLevel::Unknown;
}

bool StatusMapper::validateConfig() const
{
    bool valid = true;
    for (auto it = m_rangeRules.begin(); it != m_rangeRules.end(); ++it) {
        if (m_valueRules.contains(it.key())) {
            qWarning() << QStringLiteral("StatusMapper: field '%1' has both range and value mappings.").arg(it.key());
            valid = false;
        }
    }
    return valid;
}
