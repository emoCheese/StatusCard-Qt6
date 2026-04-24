#pragma once

#include "statuscardkit_export.h"
#include "statuscard.h"
#include <QWidget>
#include <QHash>

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

private:
    QGridLayout* m_layout = nullptr;
    int m_rows = 0;
    int m_cols = 0;
    QString m_cardClass;
    QHash<QPair<int, int>, StatusCard*> m_cards;
};
