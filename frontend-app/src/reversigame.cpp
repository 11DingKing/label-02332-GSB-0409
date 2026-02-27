// -*- coding: utf-8 -*-
#include "reversigame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

ReversiGame::ReversiGame(QWidget *parent)
    : GameBase("reversi", parent)
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

    m_countLabel = new QLabel("", this);
    m_countLabel->setObjectName("scoreLabel");

    topBar->addWidget(backBtn);
    topBar->addStretch();
    topBar->addWidget(m_statusLabel);
    topBar->addSpacing(24);
    topBar->addWidget(m_countLabel);
    topBar->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch();
}

void ReversiGame::initGame()
{
    m_gameOver = false;
    m_currentPlayer = BlackDisc;

    m_board.resize(BOARD_SIZE);
    for (int r = 0; r < BOARD_SIZE; r++) {
        m_board[r].resize(BOARD_SIZE);
        for (int c = 0; c < BOARD_SIZE; c++) {
            m_board[r][c] = Empty;
        }
    }

    int mid = BOARD_SIZE / 2;
    m_board[mid-1][mid-1] = WhiteDisc;
    m_board[mid-1][mid]   = BlackDisc;
    m_board[mid][mid-1]   = BlackDisc;
    m_board[mid][mid]     = WhiteDisc;

    countDiscs(m_blackCount, m_whiteCount);
    updateStatus();
    update();
}

void ReversiGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0F172A"));

    if (m_board.isEmpty()) return;

    int availSize = qMin(width() - 80, height() - 120);
    m_cellSize = availSize / BOARD_SIZE;
    m_cellSize = qMax(m_cellSize, 30);
    m_cellSize = qMin(m_cellSize, 70);

    int boardPx = m_cellSize * BOARD_SIZE;
    m_boardOffsetX = (width() - boardPx) / 2;
    m_boardOffsetY = (height() - boardPx) / 2 + 20;

    // Board background
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#065F46"));
    p.drawRoundedRect(m_boardOffsetX - 4, m_boardOffsetY - 4, boardPx + 8, boardPx + 8, 6, 6);

    // Grid
    p.setPen(QPen(QColor("#047857"), 1));
    for (int r = 0; r <= BOARD_SIZE; r++) {
        int y = m_boardOffsetY + r * m_cellSize;
        p.drawLine(m_boardOffsetX, y, m_boardOffsetX + boardPx, y);
    }
    for (int c = 0; c <= BOARD_SIZE; c++) {
        int x = m_boardOffsetX + c * m_cellSize;
        p.drawLine(x, m_boardOffsetY, x, m_boardOffsetY + boardPx);
    }

    // Hint box
    if (!m_gameOver) {
        QString hint = "  点击落子    半透明圆点为可落位置  ";
        QFont hintFont;
        hintFont.setPixelSize(13);
        hintFont.setBold(true);
        p.setFont(hintFont);
        QFontMetrics fm(hintFont);
        int hw = fm.horizontalAdvance(hint);
        int hh = 28;
        int hx = (width() - hw) / 2;
        int hy = m_boardOffsetY + m_cellSize * BOARD_SIZE + 10;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(30, 41, 59, 220));
        p.drawRoundedRect(hx - 10, hy, hw + 20, hh, 6, 6);
        p.setPen(QColor("#A855F7"));
        p.drawText(hx, hy + 19, hint);
    }

    double discR = m_cellSize * 0.38;

    // Valid move indicators
    if (!m_gameOver) {
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (isValidMove(r, c, m_currentPlayer)) {
                    QRect cr = cellRect(r, c);
                    QPointF center(cr.center());
                    p.setPen(Qt::NoPen);
                    p.setBrush(QColor(255, 255, 255, 30));
                    p.drawEllipse(center, discR * 0.5, discR * 0.5);
                }
            }
        }
    }

    // Discs
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (m_board[r][c] == Empty) continue;
            QRect cr = cellRect(r, c);
            QPointF center(cr.center());

            if (m_board[r][c] == BlackDisc) {
                QRadialGradient grad(center - QPointF(discR*0.2, discR*0.2), discR * 1.3);
                grad.setColorAt(0, QColor("#4B5563"));
                grad.setColorAt(1, QColor("#111827"));
                p.setBrush(grad);
                p.setPen(QPen(QColor("#1F2937"), 0.5));
            } else {
                QRadialGradient grad(center - QPointF(discR*0.2, discR*0.2), discR * 1.3);
                grad.setColorAt(0, QColor("#FFFFFF"));
                grad.setColorAt(1, QColor("#D1D5DB"));
                p.setBrush(grad);
                p.setPen(QPen(QColor("#9CA3AF"), 0.5));
            }
            p.drawEllipse(center, discR, discR);
        }
    }
}

void ReversiGame::mousePressEvent(QMouseEvent *event)
{
    if (m_gameOver || !m_running || event->button() != Qt::LeftButton) return;

    QPoint cell = cellFromPos(event->pos());
    if (cell.x() < 0) return;

    int r = cell.y(), c = cell.x();
    if (!isValidMove(r, c, m_currentPlayer)) return;

    auto flips = getFlippable(r, c, m_currentPlayer);
    m_board[r][c] = m_currentPlayer;
    for (const auto &fp : flips) {
        m_board[fp.y()][fp.x()] = m_currentPlayer;
    }

    Disc opponent = (m_currentPlayer == BlackDisc) ? WhiteDisc : BlackDisc;

    if (hasValidMoves(opponent)) {
        m_currentPlayer = opponent;
    } else if (!hasValidMoves(m_currentPlayer)) {
        m_gameOver = true;
        countDiscs(m_blackCount, m_whiteCount);
        QString result;
        if (m_blackCount > m_whiteCount) result = "win";
        else if (m_blackCount < m_whiteCount) result = "lose";
        else result = "draw";
        updateStatus();
        update();
        QString extra = QString("{\"black\":%1,\"white\":%2}").arg(m_blackCount).arg(m_whiteCount);
        finishGame(m_blackCount, result, extra);
        return;
    }

    countDiscs(m_blackCount, m_whiteCount);
    updateStatus();
    update();
}

bool ReversiGame::isValidMove(int row, int col, Disc disc) const
{
    if (m_board[row][col] != Empty) return false;
    return !getFlippable(row, col, disc).isEmpty();
}

QList<QPoint> ReversiGame::getFlippable(int row, int col, Disc disc) const
{
    QList<QPoint> all;
    static const int dirs[][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    for (auto &d : dirs) {
        all.append(getFlippableInDir(row, col, d[0], d[1], disc));
    }
    return all;
}

QList<QPoint> ReversiGame::getFlippableInDir(int row, int col, int dr, int dc, Disc disc) const
{
    QList<QPoint> line;
    Disc opponent = (disc == BlackDisc) ? WhiteDisc : BlackDisc;
    int r = row + dr, c = col + dc;

    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
        if (m_board[r][c] == opponent) {
            line.append(QPoint(c, r));
        } else if (m_board[r][c] == disc) {
            return line;
        } else {
            return {};
        }
        r += dr;
        c += dc;
    }
    return {};
}

bool ReversiGame::hasValidMoves(Disc disc) const
{
    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            if (isValidMove(r, c, disc)) return true;
    return false;
}

void ReversiGame::countDiscs(int &black, int &white) const
{
    black = 0; white = 0;
    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (m_board[r][c] == BlackDisc) black++;
            else if (m_board[r][c] == WhiteDisc) white++;
        }
}

void ReversiGame::updateStatus()
{
    if (m_gameOver) {
        if (m_blackCount > m_whiteCount) m_statusLabel->setText("黑方获胜");
        else if (m_blackCount < m_whiteCount) m_statusLabel->setText("白方获胜");
        else m_statusLabel->setText("平局");
    } else {
        m_statusLabel->setText((m_currentPlayer == BlackDisc) ? "黑方落子" : "白方落子");
    }
    m_countLabel->setText(QString("黑 %1 : %2 白").arg(m_blackCount).arg(m_whiteCount));
}

QPoint ReversiGame::cellFromPos(const QPoint &pos) const
{
    int c = (pos.x() - m_boardOffsetX) / m_cellSize;
    int r = (pos.y() - m_boardOffsetY) / m_cellSize;
    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE)
        return QPoint(-1, -1);
    return QPoint(c, r);
}

QRect ReversiGame::cellRect(int row, int col) const
{
    return QRect(m_boardOffsetX + col * m_cellSize,
                 m_boardOffsetY + row * m_cellSize,
                 m_cellSize, m_cellSize);
}

void ReversiGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
