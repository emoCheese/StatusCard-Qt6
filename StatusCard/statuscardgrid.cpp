#include "statuscardgrid.h"
#include <QGridLayout>

StatusCardGrid::StatusCardGrid(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setSpacing(12);
    m_layout->setContentsMargins(16, 16, 16, 16);
    setProperty("class", QStringLiteral("status-card-grid"));
}

StatusCardGrid::~StatusCardGrid() = default;

void StatusCardGrid::setGridSize(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;

    for (int i = 0; i < m_layout->rowCount(); ++i) {
        m_layout->setRowStretch(i, 0);
    }
    for (int j = 0; j < m_layout->columnCount(); ++j) {
        m_layout->setColumnStretch(j, 0);
    }

    for (int i = 0; i < rows; ++i) {
        m_layout->setRowStretch(i, 1);
    }
    for (int j = 0; j < cols; ++j) {
        m_layout->setColumnStretch(j, 1);
    }
}

StatusCard* StatusCardGrid::addCard(int row, int col, StatusCard* card)
{
    if (!card)
        return nullptr;

    if (!m_cardClass.isEmpty()) {
        card->setProperty("class", m_cardClass);
    }

    m_layout->addWidget(card, row, col);
    m_cards[qMakePair(row, col)] = card;
    return card;
}

StatusCard* StatusCardGrid::addCard(int row, int col, StatusCard::Builder builder)
{
    StatusCard* card = std::move(builder).build(this);
    return addCard(row, col, card);
}

StatusCard* StatusCardGrid::cardAt(int row, int col) const
{
    return m_cards.value(qMakePair(row, col), nullptr);
}

void StatusCardGrid::setCardClass(const QString& className)
{
    m_cardClass = className;
}
