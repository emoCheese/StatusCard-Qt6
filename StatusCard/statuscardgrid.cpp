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

    if (m_globalTitleFont)
        card->setTitleFont(m_globalTitleFont.value());
    if (m_globalLabelFont)
        card->setLabelFont(m_globalLabelFont.value());
    if (m_globalValueFont)
        card->setValueFont(m_globalValueFont.value());
    if (m_globalBackgroundColor)
        card->setBackgroundColor(m_globalBackgroundColor.value());

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

void StatusCardGrid::setCardTitleFont(const QFont& font)
{
    m_globalTitleFont = font;
    for (auto* card : std::as_const(m_cards)) {
        if (card)
            card->setTitleFont(font);
    }
}

void StatusCardGrid::setCardTitlePointSize(int pointSize)
{
    QFont font = m_globalTitleFont.value_or(QFont());
    font.setPointSize(pointSize);
    setCardTitleFont(font);
}

void StatusCardGrid::setCardLabelFont(const QFont& font)
{
    m_globalLabelFont = font;
    for (auto* card : std::as_const(m_cards)) {
        if (card)
            card->setLabelFont(font);
    }
}

void StatusCardGrid::setCardLabelPointSize(int pointSize)
{
    QFont font = m_globalLabelFont.value_or(QFont());
    font.setPointSize(pointSize);
    setCardLabelFont(font);
}

void StatusCardGrid::setCardValueFont(const QFont& font)
{
    m_globalValueFont = font;
    for (auto* card : std::as_const(m_cards)) {
        if (card)
            card->setValueFont(font);
    }
}

void StatusCardGrid::setCardValuePointSize(int pointSize)
{
    QFont font = m_globalValueFont.value_or(QFont());
    font.setPointSize(pointSize);
    setCardValueFont(font);
}

void StatusCardGrid::setCardBackgroundColor(const QColor& color)
{
    m_globalBackgroundColor = color;
    for (auto* card : std::as_const(m_cards)) {
        if (card)
            card->setBackgroundColor(color);
    }
}
