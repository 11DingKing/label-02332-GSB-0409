// -*- coding: utf-8 -*-
#include "gomokugame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

GomokuGame::GomokuGame(QWidget *parent)
    : GameBase("gomoku", parent)
{
    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(16, 8, 16, 0);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        resetGame();
        emit backRequested();
    });

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setObjectName("scoreLabel");

    topBar->addWidget(backBtn);
    topBar->addStretch();
    topBar->addWidget(m_statusLabel);
    topBar->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch();
}

void GomokuGame::initGame()
{
    m_gameOver = false;
    m_currentPlayer = Black;
    m_lastMove = QPoint(-1, -1);

    switch (m_difficulty) {
    case Easy:   m_boardSize = 15; break;
    case Medium: m_boardSize = 15; break;
    case Hard:   m_boardSize = 19; break;
    case Custom:
        m_boardSize = m_customParams.value("board_size", 15).toInt();
        break;
    }

    m_board.resize(m_boardSize);
    for (int r = 0; r < m_boardSize; r++) {
        m_board[r].resize(m_boardSize);
        for (int c = 0; c < m_boardSize; c++) {
            m_board[r][c] = Empty;
        }
    }

    m_statusLabel->setText("黑方落子");
    update();
}

void GomokuGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0F172A"));

    if (m_board.isEmpty()) return;

    int availSize = qMin(width() - 80, height() - 120);
    m_gridSize = (double)availSize / (m_boardSize + 1);
    m_gridSize = qMax(m_gridSize, 12.0);

    double boardLen = m_gridSize * (m_boardSize - 1);
    m_boardOffsetX = (width() - boardLen) / 2.0;
    m_boardOffsetY = (height() - boardLen) / 2.0 + 20;

    // Board background
    double pad = m_gridSize * 0.7;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#2D1B0E"));
    p.drawRoundedRect(QRectF(m_boardOffsetX - pad, m_boardOffsetY - pad,
                              boardLen + 2 * pad, boardLen + 2 * pad), 8, 8);

    // Grid lines
    p.setPen(QPen(QColor("#5C3A1E"), 1));
    for (int i = 0; i < m_boardSize; i++) {
        double y = m_boardOffsetY + i * m_gridSize;
        p.drawLine(QPointF(m_boardOffsetX, y), QPointF(m_boardOffsetX + boardLen, y));
        double x = m_boardOffsetX + i * m_gridSize;
        p.drawLine(QPointF(x, m_boardOffsetY), QPointF(x, m_boardOffsetY + boardLen));
    }

    // Star points
    QList<QPoint> starPts;
    if (m_boardSize == 15) {
        starPts = {{3,3},{3,7},{3,11},{7,3},{7,7},{7,11},{11,3},{11,7},{11,11}};
    } else if (m_boardSize == 13) {
        starPts = {{3,3},{3,9},{6,6},{9,3},{9,9}};
    } else if (m_boardSize == 19) {
        starPts = {{3,3},{3,9},{3,15},{9,3},{9,9},{9,15},{15,3},{15,9},{15,15}};
    }
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#5C3A1E"));
    for (const auto &sp : starPts) {
        QPointF center = intersectionCenter(sp.y(), sp.x());
        p.drawEllipse(center, 3, 3);
    }

    // Hint box
    if (!m_gameOver) {
        QString hint = "  点击棋盘交叉点落子    五子连珠获胜  ";
        QFont hintFont;
        hintFont.setPixelSize(13);
        hintFont.setBold(true);
        p.setFont(hintFont);
        QFontMetrics fm(hintFont);
        int hw = fm.horizontalAdvance(hint);
        int hh = 28;
        double boardLen = m_gridSize * (m_boardSize - 1);
        int hx = (width() - hw) / 2;
        int hy = (int)(m_boardOffsetY + boardLen) + 12;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(30, 41, 59, 220));
        p.drawRoundedRect(hx - 10, hy, hw + 20, hh, 6, 6);
        p.setPen(QColor("#FBBF24"));
        p.drawText(hx, hy + 19, hint);
    }

    // Stones
    double stoneRadius = m_gridSize * 0.42;
    for (int r = 0; r < m_boardSize; r++) {
        for (int c = 0; c < m_boardSize; c++) {
            if (m_board[r][c] == Empty) continue;
            QPointF center = intersectionCenter(r, c);

            if (m_board[r][c] == Black) {
                QRadialGradient grad(center - QPointF(stoneRadius*0.3, stoneRadius*0.3),
                                     stoneRadius * 1.5);
                grad.setColorAt(0, QColor("#4B5563"));
                grad.setColorAt(1, QColor("#111827"));
                p.setBrush(grad);
                p.setPen(QPen(QColor("#1F2937"), 0.5));
            } else {
                QRadialGradient grad(center - QPointF(stoneRadius*0.3, stoneRadius*0.3),
                                     stoneRadius * 1.5);
                grad.setColorAt(0, QColor("#FFFFFF"));
                grad.setColorAt(1, QColor("#D1D5DB"));
                p.setBrush(grad);
                p.setPen(QPen(QColor("#9CA3AF"), 0.5));
            }
            p.drawEllipse(center, stoneRadius, stoneRadius);

            if (r == m_lastMove.y() && c == m_lastMove.x()) {
                p.setPen(QPen(QColor("#EF4444"), 2));
                p.setBrush(Qt::NoBrush);
                p.drawEllipse(center, stoneRadius * 0.4, stoneRadius * 0.4);
            }
        }
    }
}

void GomokuGame::mousePressEvent(QMouseEvent *event)
{
    if (m_gameOver || !m_running || event->button() != Qt::LeftButton) return;

    QPoint cell = nearestIntersection(event->pos());
    if (cell.x() < 0) return;

    int r = cell.y(), c = cell.x();
    if (m_board[r][c] != Empty) return;

    m_board[r][c] = m_currentPlayer;
    m_lastMove = QPoint(c, r);

    if (checkWin(r, c, m_currentPlayer)) {
        m_gameOver = true;
        QString winner = (m_currentPlayer == Black) ? "black" : "white";
        QString result = (m_currentPlayer == Black) ? "win" : "lose";
        m_statusLabel->setText((m_currentPlayer == Black) ? "黑方获胜" : "白方获胜");
        update();
        QString extra = QString("{\"board_size\":%1,\"winner\":\"%2\"}").arg(m_boardSize).arg(winner);
        finishGame(0, result, extra);
        return;
    }

    // Check draw
    bool full = true;
    for (int r2 = 0; r2 < m_boardSize && full; r2++)
        for (int c2 = 0; c2 < m_boardSize && full; c2++)
            if (m_board[r2][c2] == Empty) full = false;

    if (full) {
        m_gameOver = true;
        m_statusLabel->setText("平局");
        update();
        QString drawExtra = QString("{\"board_size\":%1,\"winner\":\"none\"}").arg(m_boardSize);
        finishGame(0, "draw", drawExtra);
        return;
    }

    m_currentPlayer = (m_currentPlayer == Black) ? White : Black;
    m_statusLabel->setText((m_currentPlayer == Black) ? "黑方落子" : "白方落子");
    update();
}

bool GomokuGame::checkWin(int row, int col, Stone stone) const
{
    static const int dirs[][2] = {{0,1},{1,0},{1,1},{1,-1}};
    for (auto &d : dirs) {
        int count = 1 + countDirection(row, col, d[0], d[1], stone)
                      + countDirection(row, col, -d[0], -d[1], stone);
        if (count >= 5) return true;
    }
    return false;
}

int GomokuGame::countDirection(int row, int col, int dr, int dc, Stone stone) const
{
    int count = 0;
    int r = row + dr, c = col + dc;
    while (r >= 0 && r < m_boardSize && c >= 0 && c < m_boardSize && m_board[r][c] == stone) {
        count++;
        r += dr;
        c += dc;
    }
    return count;
}

QPoint GomokuGame::nearestIntersection(const QPoint &pos) const
{
    double bestDist = m_gridSize;
    QPoint best(-1, -1);
    for (int r = 0; r < m_boardSize; r++) {
        for (int c = 0; c < m_boardSize; c++) {
            QPointF center = intersectionCenter(r, c);
            double dx = pos.x() - center.x();
            double dy = pos.y() - center.y();
            double dist = qSqrt(dx*dx + dy*dy);
            if (dist < bestDist) {
                bestDist = dist;
                best = QPoint(c, r);
            }
        }
    }
    return best;
}

QPointF GomokuGame::intersectionCenter(int row, int col) const
{
    return QPointF(m_boardOffsetX + col * m_gridSize,
                   m_boardOffsetY + row * m_gridSize);
}

void GomokuGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
