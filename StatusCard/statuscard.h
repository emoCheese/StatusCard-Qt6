#pragma once

#include "statuscardkit_export.h"
#include "statusmapper.h"
#include <QWidget>
#include <QMap>
#include <QVariant>
#include <memory>

class QLabel;
class QVBoxLayout;

class STATUSCARDKIT_EXPORT StatusCard : public QWidget
{
    Q_OBJECT
public:
    static constexpr StatusLevel Normal = StatusLevel::Normal;
    static constexpr StatusLevel Warning = StatusLevel::Warning;
    static constexpr StatusLevel Error = StatusLevel::Error;
    static constexpr StatusLevel Unknown = StatusLevel::Unknown;

    class Builder
    {
    public:
        static Builder create() { return Builder(); }

        Builder& title(const QString& t)
        {
            m_title = t;
            return *this;
        }

        Builder& index(int idx)
        {
            m_index = idx;
            return *this;
        }

        Builder& addField(const QString& label, const QString& defaultValue = QString())
        {
            m_fields.append(qMakePair(label, defaultValue));
            return *this;
        }

        Builder& mapRange(const QString& field, int min, int max, StatusLevel level)
        {
            m_rangeRules.append(std::make_tuple(field, min, max, level));
            return *this;
        }

        Builder& mapValue(const QString& field, const QVariant& value, StatusLevel level)
        {
            m_valueRules.append(std::make_tuple(field, value, level));
            return *this;
        }

        Builder& setUnit(const QString& field, const QString& unit)
        {
            m_units.insert(field, unit);
            return *this;
        }

        StatusCard* build(QWidget* parent = nullptr) &&
        {
            StatusCard* card = new StatusCard(parent);
            card->m_title = m_title;
            card->m_index = m_index;
            for (const auto& f : std::as_const(m_fields)) {
                card->addFieldWidget(f.first, f.second);
            }
            for (const auto& r : std::as_const(m_rangeRules)) {
                card->m_mapper->addRangeRule(std::get<0>(r), std::get<1>(r), std::get<2>(r), std::get<3>(r));
            }
            for (const auto& v : std::as_const(m_valueRules)) {
                card->m_mapper->addValueRule(std::get<0>(v), std::get<1>(v), std::get<2>(v));
            }
            card->m_fieldUnits = m_units;
            card->updateTitle();
            return card;
        }

    private:
        Builder() = default;
        QString m_title;
        int m_index = 0;
        QVector<QPair<QString, QString>> m_fields;
        QVector<std::tuple<QString, int, int, StatusLevel>> m_rangeRules;
        QVector<std::tuple<QString, QVariant, StatusLevel>> m_valueRules;
        QHash<QString, QString> m_units;
    };

    explicit StatusCard(QWidget* parent = nullptr);
    ~StatusCard() override;

    static Builder create() { return Builder::create(); }

    void updateField(const QString& fieldName, const QVariant& value);
    void updateFields(const QMap<QString, QVariant>& values);

    bool validateConfig() const;

    QString title() const { return m_title; }
    int index() const { return m_index; }

    void setTitleFont(const QFont& font);
    void setTitlePointSize(int pointSize);

    void setLabelFont(const QFont& font);
    void setLabelPointSize(int pointSize);

    void setValueFont(const QFont& font);
    void setValuePointSize(int pointSize);

    void setBackgroundColor(const QColor& color);

private:
    friend class Builder;

    void updateFieldInternal(const QString& fieldName, const QVariant& value);
    void updateCardLevel();
    void updateTitle();
    void addFieldWidget(const QString& label, const QString& defaultValue);
    void applyTitleFont();
    void applyLabelFont();
    void applyValueFont();
    static QColor levelToColor(StatusLevel level);
    static QString levelToString(StatusLevel level);

    struct FieldWidget {
        QLabel* label = nullptr;
        QLabel* value = nullptr;
        StatusLevel currentLevel = StatusLevel::Unknown;
    };

    std::unique_ptr<StatusMapper> m_mapper;
    QString m_title;
    int m_index = 0;
    QHash<QString, FieldWidget> m_fields;
    QHash<QString, QString> m_fieldUnits;
    QLabel* m_titleLabel = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;
    QFont m_titleFont;
    QFont m_labelFont;
    QFont m_valueFont;
    QColor m_backgroundColor;
};
