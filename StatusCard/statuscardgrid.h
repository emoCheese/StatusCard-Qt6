#pragma once

#include "statuscardkit_export.h"
#include "statuscard.h"
#include <QWidget>
#include <QHash>
#include <QFont>
#include <optional>

class QGridLayout;

class STATUSCARDKIT_EXPORT StatusCardGrid : public QWidget
{
    Q_OBJECT
public:
    explicit StatusCardGrid(QWidget* parent = nullptr);
    ~StatusCardGrid() override;

    void setGridSize(int rows, int cols);

    StatusCard* addCard(int row, int col, StatusCard* card);
    StatusCard* addCard(int row, int col, StatusCard::Builder builder);

    StatusCard* cardAt(int row, int col) const;

    void setCardClass(const QString& className);

    void setCardTitleFont(const QFont& font);
    void setCardTitlePointSize(int pointSize);

    void setCardLabelFont(const QFont& font);
    void setCardLabelPointSize(int pointSize);

    void setCardValueFont(const QFont& font);
    void setCardValuePointSize(int pointSize);

private:
    QGridLayout* m_layout = nullptr;
    int m_rows = 0;
    int m_cols = 0;
    QString m_cardClass;
    QHash<QPair<int, int>, StatusCard*> m_cards;
    std::optional<QFont> m_globalTitleFont;
    std::optional<QFont> m_globalLabelFont;
    std::optional<QFont> m_globalValueFont;
};
