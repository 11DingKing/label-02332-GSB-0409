// -*- coding: utf-8 -*-
#include "snakegame.h"
#include <QPainter>
#include <QKeyEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>

SnakeGame::SnakeGame(QWidget *parent)
    : GameBase("snake", parent)
{
    m_moveTimer = new QTimer(this);
    connect(m_moveTimer, &QTimer::timeout, this, &SnakeGame::tick);

    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(16, 8, 16, 0);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        m_moveTimer->stop();
        resetGame();
        emit backRequested();
    });

    m_scoreLabel = new QLabel("得分: 0", this);
    m_scoreLabel->setObjectName("scoreLabel");

    topBar->addWidget(backBtn);
    topBar->addStretch();
    topBar->addWidget(m_scoreLabel);
    topBar->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch();
}

void SnakeGame::pauseGame()
{
    GameBase::pauseGame();
    m_moveTimer->stop();
}

void SnakeGame::resumeGame()
{
    GameBase::resumeGame();
    if (m_running && !m_gameOver) {
        m_moveTimer->start(m_speed);
    }
}

void SnakeGame::initGame()
{
    m_gameOver = false;
    m_score = 0;
    m_direction = Right;
    m_nextDirection = Right;

    switch (m_difficulty) {
    case Easy:   m_speed = 200; break;
    case Medium: m_speed = 150; break;
    case Hard:   m_speed = 100; break;
    case Custom:
        m_speed = m_customParams.value("speed", 150).toInt();
        break;
    }

    m_snake.clear();
    int startRow = GRID_ROWS / 2;
    int startCol = GRID_COLS / 2;
    m_snake.append(QPoint(startCol, startRow));
    m_snake.append(QPoint(startCol - 1, startRow));
    m_snake.append(QPoint(startCol - 2, startRow));

    spawnFood();

    m_scoreLabel->setText("得分: 0");
    m_moveTimer->start(m_speed);
    update();
}

void SnakeGame::tick()
{
    if (m_gameOver || m_paused) return;

    m_direction = m_nextDirection;

    QPoint head = m_snake.first();
    QPoint newHead = head;

    switch (m_direction) {
    case Up:    newHead.ry()--; break;
    case Down:  newHead.ry()++; break;
    case Left:  newHead.rx()--; break;
    case Right: newHead.rx()++; break;
    }

    if (newHead.x() < 0 || newHead.x() >= GRID_COLS ||
        newHead.y() < 0 || newHead.y() >= GRID_ROWS) {
        m_gameOver = true;
        m_moveTimer->stop();
        update();
        QString extra = QString("{\"speed\":%1,\"length\":%2}").arg(m_speed).arg(m_snake.size());
        finishGame(m_score, "score_only", extra);
        return;
    }

    if (m_snake.contains(newHead)) {
        m_gameOver = true;
        m_moveTimer->stop();
        update();
        QString extra = QString("{\"speed\":%1,\"length\":%2}").arg(m_speed).arg(m_snake.size());
        finishGame(m_score, "score_only", extra);
        return;
    }

    m_snake.prepend(newHead);

    if (newHead == m_food) {
        m_score += 10;
        m_scoreLabel->setText(QString("得分: %1").arg(m_score));
        spawnFood();
    } else {
        m_snake.removeLast();
    }

    update();
}

void SnakeGame::spawnFood()
{
    auto *rng = QRandomGenerator::global();
    QPoint food;
    do {
        food = QPoint(rng->bounded(GRID_COLS), rng->bounded(GRID_ROWS));
    } while (m_snake.contains(food));
    m_food = food;
}

void SnakeGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0F172A"));

    int availW = width() - 40;
    int availH = height() - 80;
    m_cellSize = qMin(availW / GRID_COLS, availH / GRID_ROWS);
    m_cellSize = qMax(m_cellSize, 12);
    m_cellSize = qMin(m_cellSize, 36);

    int boardW = m_cellSize * GRID_COLS;
    int boardH = m_cellSize * GRID_ROWS;
    m_boardOffsetX = (width() - boardW) / 2;
    m_boardOffsetY = (height() - boardH) / 2 + 20;

    // Board background
    p.setPen(QColor("#1E293B"));
    p.setBrush(QColor("#1E293B"));
    p.drawRoundedRect(m_boardOffsetX - 2, m_boardOffsetY - 2, boardW + 4, boardH + 4, 6, 6);

    // Grid lines
    p.setPen(QColor(51, 65, 85, 40));
    for (int r = 0; r <= GRID_ROWS; r++) {
        int y = m_boardOffsetY + r * m_cellSize;
        p.drawLine(m_boardOffsetX, y, m_boardOffsetX + boardW, y);
    }
    for (int c = 0; c <= GRID_COLS; c++) {
        int x = m_boardOffsetX + c * m_cellSize;
        p.drawLine(x, m_boardOffsetY, x, m_boardOffsetY + boardH);
    }

    // Food
    QRect foodRect = cellRect(m_food.y(), m_food.x()).adjusted(3, 3, -3, -3);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#EF4444"));
    p.drawRoundedRect(foodRect, 4, 4);

    // Snake
    for (int i = 0; i < m_snake.size(); i++) {
        QRect sr = cellRect(m_snake[i].y(), m_snake[i].x()).adjusted(1, 1, -1, -1);
        if (i == 0) {
            p.setBrush(QColor("#22D3EE"));
        } else {
            p.setBrush(QColor("#10B981"));
        }
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(sr, 4, 4);
    }

    // Keyboard hint box
    if (!m_gameOver) {
        QString hint = "  ↑ W  ↓ S  ← A  → D  移动方向    空格  暂停 / 继续  ";
        QFont hintFont;
        hintFont.setPixelSize(13);
        hintFont.setBold(true);
        p.setFont(hintFont);
        QFontMetrics fm(hintFont);
        int hw = fm.horizontalAdvance(hint);
        int hh = 28;
        int hx = (width() - hw) / 2;
        int hy = m_boardOffsetY + boardH + 10;
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(30, 41, 59, 220));
        p.drawRoundedRect(hx - 10, hy, hw + 20, hh, 6, 6);
        p.setPen(QColor("#22D3EE"));
        p.drawText(hx, hy + 19, hint);
    }

    if (m_gameOver) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(15, 23, 42, 180));
        p.drawRect(rect());

        p.setPen(QColor("#EF4444"));
        QFont gameOverFont;
        gameOverFont.setPixelSize(36);
        gameOverFont.setBold(true);
        p.setFont(gameOverFont);
        p.drawText(rect(), Qt::AlignCenter, "游戏结束");
    }
}

void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    if (m_gameOver) return;

    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (m_direction != Down) m_nextDirection = Up;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (m_direction != Up) m_nextDirection = Down;
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (m_direction != Right) m_nextDirection = Left;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (m_direction != Left) m_nextDirection = Right;
        break;
    case Qt::Key_Space:
        if (m_paused) resumeGame();
        else pauseGame();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void SnakeGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

QRect SnakeGame::cellRect(int row, int col) const
{
    return QRect(m_boardOffsetX + col * m_cellSize,
                 m_boardOffsetY + row * m_cellSize,
                 m_cellSize, m_cellSize);
}
