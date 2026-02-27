// -*- coding: utf-8 -*-
#include "minesweepergame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QSvgRenderer>

MinesweeperGame::MinesweeperGame(QWidget *parent)
    : GameBase("minesweeper", parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        if (m_running && !m_paused && !m_gameOver) {
            m_elapsedSeconds++;
            m_timeLabel->setText(QString("时间: %1s").arg(m_elapsedSeconds));
        }
    });

    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(16, 8, 16, 0);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        m_timer->stop();
        resetGame();
        emit backRequested();
    });

    m_timeLabel = new QLabel("时间: 0s", this);
    m_timeLabel->setObjectName("scoreLabel");
    m_mineLabel = new QLabel("剩余: 0", this);
    m_mineLabel->setObjectName("scoreLabel");

    topBar->addWidget(backBtn);
    topBar->addStretch();
    topBar->addWidget(m_timeLabel);
    topBar->addSpacing(24);
    topBar->addWidget(m_mineLabel);
    topBar->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch();
}

void MinesweeperGame::pauseGame()
{
    GameBase::pauseGame();
    m_timer->stop();
}

void MinesweeperGame::resumeGame()
{
    GameBase::resumeGame();
    if (m_running && !m_gameOver) {
        m_timer->start(1000);
    }
}

void MinesweeperGame::initGame()
{
    m_gameOver = false;
    m_firstClick = true;
    m_elapsedSeconds = 0;
    m_flagCount = 0;
    m_revealedCount = 0;

    switch (m_difficulty) {
    case Easy:   m_rows = 9;  m_cols = 9;  m_mineCount = 10; break;
    case Medium: m_rows = 16; m_cols = 16; m_mineCount = 40; break;
    case Hard:   m_rows = 16; m_cols = 30; m_mineCount = 99; break;
    case Custom:
        m_rows = m_customParams.value("rows", 9).toInt();
        m_cols = m_customParams.value("cols", 9).toInt();
        m_mineCount = m_customParams.value("mines", 10).toInt();
        if (m_mineCount >= m_rows * m_cols)
            m_mineCount = m_rows * m_cols - 1;
        break;
    }

    m_grid.resize(m_rows);
    for (int r = 0; r < m_rows; r++) {
        m_grid[r].resize(m_cols);
        for (int c = 0; c < m_cols; c++) {
            m_grid[r][c] = Cell();
        }
    }

    m_mineLabel->setText(QString("剩余: %1").arg(m_mineCount));
    m_timeLabel->setText("时间: 0s");
    m_timer->start(1000);

    update();
}

void MinesweeperGame::generateMines(int firstRow, int firstCol)
{
    int placed = 0;
    auto *rng = QRandomGenerator::global();
    while (placed < m_mineCount) {
        int r = rng->bounded(m_rows);
        int c = rng->bounded(m_cols);
        if (m_grid[r][c].hasMine) continue;
        if (qAbs(r - firstRow) <= 1 && qAbs(c - firstCol) <= 1) continue;
        m_grid[r][c].hasMine = true;
        placed++;
    }

    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            if (!m_grid[r][c].hasMine) {
                m_grid[r][c].adjacentMines = countAdjacentMines(r, c);
            }
        }
    }
}

int MinesweeperGame::countAdjacentMines(int row, int col) const
{
    int count = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = row + dr, nc = col + dc;
            if (nr >= 0 && nr < m_rows && nc >= 0 && nc < m_cols && m_grid[nr][nc].hasMine)
                count++;
        }
    }
    return count;
}

void MinesweeperGame::revealCell(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols) return;
    Cell &cell = m_grid[row][col];
    if (cell.state != Hidden) return;

    cell.state = Revealed;
    m_revealedCount++;

    if (cell.hasMine) {
        m_gameOver = true;
        m_timer->stop();
        revealAllMines();
        update();
        QString extra = QString("{\"rows\":%1,\"cols\":%2,\"mines\":%3,\"time\":%4}")
                            .arg(m_rows).arg(m_cols).arg(m_mineCount).arg(m_elapsedSeconds);
        finishGame(m_elapsedSeconds, "lose", extra);
        return;
    }

    if (cell.adjacentMines == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                revealCell(row + dr, col + dc);
            }
        }
    }
}

void MinesweeperGame::revealAllMines()
{
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            if (m_grid[r][c].hasMine) {
                m_grid[r][c].state = Revealed;
            }
        }
    }
}

void MinesweeperGame::checkWinCondition()
{
    if (m_revealedCount == m_rows * m_cols - m_mineCount) {
        m_gameOver = true;
        m_timer->stop();
        update();
        QString extra = QString("{\"rows\":%1,\"cols\":%2,\"mines\":%3,\"time\":%4}")
                            .arg(m_rows).arg(m_cols).arg(m_mineCount).arg(m_elapsedSeconds);
        finishGame(m_elapsedSeconds, "win", extra);
    }
}

void MinesweeperGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor("#0F172A"));

    if (m_grid.isEmpty()) return;

    int availW = width() - 40;
    int availH = height() - 80;
    m_cellSize = qMin(availW / m_cols, availH / m_rows);
    m_cellSize = qMax(m_cellSize, 16);
    m_cellSize = qMin(m_cellSize, 40);

    int boardW = m_cellSize * m_cols;
    int boardH = m_cellSize * m_rows;
    m_boardOffsetX = (width() - boardW) / 2;
    m_boardOffsetY = (height() - boardH) / 2 + 20;

    static const QColor numColors[] = {
        QColor("#3B82F6"), QColor("#10B981"), QColor("#EF4444"), QColor("#8B5CF6"),
        QColor("#F59E0B"), QColor("#06B6D4"), QColor("#F1F5F9"), QColor("#94A3B8")
    };

    // Keyboard / mouse hint box
    if (!m_gameOver) {
        QString hint = "  左键  揭开格子    右键  插旗 / 取消旗  ";
        QFont hintFont;
        hintFont.setPixelSize(13);
        hintFont.setBold(true);
        p.setFont(hintFont);
        QFontMetrics fm(hintFont);
        int hw = fm.horizontalAdvance(hint);
        int hh = 28;
        int hx = (width() - hw) / 2;
        int hy = m_boardOffsetY + m_cellSize * m_rows + 10;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(30, 41, 59, 220));
        p.drawRoundedRect(hx - 10, hy, hw + 20, hh, 6, 6);
        p.setPen(QColor("#EF4444"));
        p.drawText(hx, hy + 19, hint);
    }

    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            QRect cr = cellRect(r, c);
            const Cell &cell = m_grid[r][c];

            if (cell.state == Hidden) {
                p.setPen(QColor("#475569"));
                p.setBrush(QColor("#334155"));
                p.drawRoundedRect(cr.adjusted(1, 1, -1, -1), 3, 3);
            } else if (cell.state == Flagged) {
                p.setPen(QColor("#475569"));
                p.setBrush(QColor("#334155"));
                p.drawRoundedRect(cr.adjusted(1, 1, -1, -1), 3, 3);
                QSvgRenderer flagSvg(QString(":/icons/flag.svg"));
                int iconSize = m_cellSize * 0.6;
                QRect iconRect(cr.center().x() - iconSize/2, cr.center().y() - iconSize/2,
                               iconSize, iconSize);
                flagSvg.render(&p, iconRect);
            } else {
                p.setPen(QColor("#1E293B"));
                p.setBrush(QColor("#1E293B"));
                p.drawRoundedRect(cr.adjusted(1, 1, -1, -1), 3, 3);

                if (cell.hasMine) {
                    QSvgRenderer mineSvg(QString(":/icons/mine.svg"));
                    int iconSize = m_cellSize * 0.7;
                    QRect iconRect(cr.center().x() - iconSize/2, cr.center().y() - iconSize/2,
                                   iconSize, iconSize);
                    mineSvg.render(&p, iconRect);
                } else if (cell.adjacentMines > 0) {
                    p.setPen(numColors[cell.adjacentMines - 1]);
                    QFont numFont;
                    numFont.setPixelSize(m_cellSize * 0.55);
                    numFont.setBold(true);
                    p.setFont(numFont);
                    p.drawText(cr, Qt::AlignCenter, QString::number(cell.adjacentMines));
                }
            }
        }
    }
}

void MinesweeperGame::mousePressEvent(QMouseEvent *event)
{
    if (m_gameOver || !m_running) return;

    QPoint cell = cellFromPos(event->pos());
    if (cell.x() < 0) return;

    int r = cell.y(), c = cell.x();

    if (event->button() == Qt::LeftButton) {
        if (m_grid[r][c].state == Flagged) return;
        if (m_firstClick) {
            generateMines(r, c);
            m_firstClick = false;
        }
        revealCell(r, c);
        if (!m_gameOver) checkWinCondition();
    } else if (event->button() == Qt::RightButton) {
        Cell &cell_ref = m_grid[r][c];
        if (cell_ref.state == Hidden) {
            cell_ref.state = Flagged;
            m_flagCount++;
        } else if (cell_ref.state == Flagged) {
            cell_ref.state = Hidden;
            m_flagCount--;
        }
        m_mineLabel->setText(QString("剩余: %1").arg(m_mineCount - m_flagCount));
    }

    update();
}

void MinesweeperGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

QRect MinesweeperGame::cellRect(int row, int col) const
{
    return QRect(m_boardOffsetX + col * m_cellSize,
                 m_boardOffsetY + row * m_cellSize,
                 m_cellSize, m_cellSize);
}

QPoint MinesweeperGame::cellFromPos(const QPoint &pos) const
{
    int c = (pos.x() - m_boardOffsetX) / m_cellSize;
    int r = (pos.y() - m_boardOffsetY) / m_cellSize;
    if (r < 0 || r >= m_rows || c < 0 || c >= m_cols)
        return QPoint(-1, -1);
    return QPoint(c, r);
}
