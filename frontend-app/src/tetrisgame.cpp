// -*- coding: utf-8 -*-
#include "tetrisgame.h"
#include <QPainter>
#include <QKeyEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>

// 7 Tetrominos: I, O, T, S, Z, J, L — 4 rotation states each
const QVector<QVector<QVector<QPoint>>> TetrisGame::SHAPES = {
    // I
    {{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}, {{0,2},{1,2},{2,2},{3,2}}, {{1,0},{1,1},{1,2},{1,3}}},
    // O
    {{{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}},
    // T
    {{{1,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{2,1},{1,2}}, {{1,0},{0,1},{1,1},{1,2}}},
    // S
    {{{1,0},{2,0},{0,1},{1,1}}, {{1,0},{1,1},{2,1},{2,2}}, {{1,1},{2,1},{0,2},{1,2}}, {{0,0},{0,1},{1,1},{1,2}}},
    // Z
    {{{0,0},{1,0},{1,1},{2,1}}, {{2,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{1,2},{2,2}}, {{1,0},{0,1},{1,1},{0,2}}},
    // J
    {{{0,0},{0,1},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{1,2}}, {{0,1},{1,1},{2,1},{2,2}}, {{1,0},{1,1},{0,2},{1,2}}},
    // L
    {{{2,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{0,2}}, {{0,0},{1,0},{1,1},{1,2}}},
};

const QColor TetrisGame::COLORS[7] = {
    QColor("#22D3EE"), // I - cyan
    QColor("#FBBF24"), // O - yellow
    QColor("#A855F7"), // T - purple
    QColor("#10B981"), // S - green
    QColor("#EF4444"), // Z - red
    QColor("#3B82F6"), // J - blue
    QColor("#F97316"), // L - orange
};

QVector<QPoint> TetrisGame::Piece::blocks() const
{
    return SHAPES[type][rotation % 4];
}

TetrisGame::TetrisGame(QWidget *parent)
    : GameBase("tetris", parent)
{
    m_dropTimer = new QTimer(this);
    connect(m_dropTimer, &QTimer::timeout, this, &TetrisGame::tick);

    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(16, 8, 16, 0);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        m_dropTimer->stop();
        resetGame();
        emit backRequested();
    });

    m_scoreLabel = new QLabel("得分: 0", this);
    m_scoreLabel->setObjectName("scoreLabel");
    m_levelLabel = new QLabel("等级: 1", this);
    m_levelLabel->setObjectName("scoreLabel");
    m_linesLabel = new QLabel("消行: 0", this);
    m_linesLabel->setObjectName("scoreLabel");

    topBar->addWidget(backBtn);
    topBar->addStretch();
    topBar->addWidget(m_scoreLabel);
    topBar->addSpacing(16);
    topBar->addWidget(m_levelLabel);
    topBar->addSpacing(16);
    topBar->addWidget(m_linesLabel);
    topBar->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topBar);
    mainLayout->addStretch();
}

void TetrisGame::pauseGame()
{
    GameBase::pauseGame();
    m_dropTimer->stop();
}

void TetrisGame::resumeGame()
{
    GameBase::resumeGame();
    if (m_running && !m_gameOver) {
        int speed = qMax(100, m_baseSpeed - (m_level - 1) * 40);
        m_dropTimer->start(speed);
    }
}

void TetrisGame::initGame()
{
    m_gameOver = false;
    m_score = 0;
    m_linesCleared = 0;

    switch (m_difficulty) {
    case Easy:   m_level = 1; m_baseSpeed = 600; break;
    case Medium: m_level = 3; m_baseSpeed = 400; break;
    case Hard:   m_level = 6; m_baseSpeed = 250; break;
    case Custom:
        m_level = m_customParams.value("start_level", 1).toInt();
        m_baseSpeed = qMax(100, 650 - m_level * 50);
        break;
    }

    m_field.resize(ROWS);
    for (int r = 0; r < ROWS; r++) {
        m_field[r].resize(COLS);
        for (int c = 0; c < COLS; c++) {
            m_field[r][c] = -1;
        }
    }

    m_next.type = QRandomGenerator::global()->bounded(7);
    m_next.rotation = 0;
    m_next.pos = QPoint(0, 0);

    spawnPiece();

    m_scoreLabel->setText("得分: 0");
    m_levelLabel->setText(QString("等级: %1").arg(m_level));
    m_linesLabel->setText("消行: 0");

    m_dropTimer->start(m_baseSpeed);
    update();
}

void TetrisGame::spawnPiece()
{
    m_current = m_next;
    m_current.pos = QPoint(COLS / 2 - 2, 0);

    m_next.type = QRandomGenerator::global()->bounded(7);
    m_next.rotation = 0;

    if (!canMove(m_current, 0, 0)) {
        m_gameOver = true;
        m_dropTimer->stop();
        update();
        QString extra = QString("{\"level\":%1,\"lines\":%2}").arg(m_level).arg(m_linesCleared);
        finishGame(m_score, "score_only", extra);
    }
}

void TetrisGame::tick()
{
    if (m_gameOver || m_paused) return;

    if (canMove(m_current, 0, 1)) {
        m_current.pos.ry() += 1;
    } else {
        lockPiece();
        int cleared = clearLines();
        if (cleared > 0) {
            static const int points[] = {0, 100, 300, 500, 800};
            m_score += points[cleared] * m_level;
            m_linesCleared += cleared;
            updateLevel();
            m_scoreLabel->setText(QString("得分: %1").arg(m_score));
            m_linesLabel->setText(QString("消行: %1").arg(m_linesCleared));
        }
        spawnPiece();
    }
    update();
}

bool TetrisGame::canMove(const Piece &piece, int dx, int dy) const
{
    auto blocks = piece.blocks();
    for (const auto &b : blocks) {
        int nx = piece.pos.x() + b.x() + dx;
        int ny = piece.pos.y() + b.y() + dy;
        if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return false;
        if (m_field[ny][nx] >= 0) return false;
    }
    return true;
}

bool TetrisGame::canRotate(const Piece &piece) const
{
    Piece rotated = piece;
    rotated.rotation = (rotated.rotation + 1) % 4;
    return canMove(rotated, 0, 0);
}

void TetrisGame::lockPiece()
{
    auto blocks = m_current.blocks();
    for (const auto &b : blocks) {
        int x = m_current.pos.x() + b.x();
        int y = m_current.pos.y() + b.y();
        if (y >= 0 && y < ROWS && x >= 0 && x < COLS) {
            m_field[y][x] = m_current.type;
        }
    }
}

int TetrisGame::clearLines()
{
    int cleared = 0;
    for (int r = ROWS - 1; r >= 0; r--) {
        bool full = true;
        for (int c = 0; c < COLS; c++) {
            if (m_field[r][c] < 0) { full = false; break; }
        }
        if (full) {
            m_field.removeAt(r);
            QVector<int> emptyRow(COLS, -1);
            m_field.prepend(emptyRow);
            cleared++;
            r++;
        }
    }
    return cleared;
}

void TetrisGame::updateLevel()
{
    int newLevel = 1 + m_linesCleared / 10;
    if (newLevel > m_level) {
        m_level = newLevel;
        m_levelLabel->setText(QString("等级: %1").arg(m_level));
        int speed = qMax(100, m_baseSpeed - (m_level - 1) * 40);
        m_dropTimer->setInterval(speed);
    }
}

void TetrisGame::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0F172A"));

    int availH = height() - 80;
    m_cellSize = availH / ROWS;
    m_cellSize = qMax(m_cellSize, 16);
    m_cellSize = qMin(m_cellSize, 34);

    int boardW = m_cellSize * COLS;
    int boardH = m_cellSize * ROWS;
    m_boardOffsetX = (width() - boardW) / 2 - 60;
    m_boardOffsetY = (height() - boardH) / 2 + 20;

    // Board frame
    p.setPen(QColor("#475569"));
    p.setBrush(QColor("#1E293B"));
    p.drawRoundedRect(m_boardOffsetX - 3, m_boardOffsetY - 3, boardW + 6, boardH + 6, 4, 4);

    // Grid lines
    p.setPen(QColor(51, 65, 85, 30));
    for (int r = 0; r <= ROWS; r++) {
        int y = m_boardOffsetY + r * m_cellSize;
        p.drawLine(m_boardOffsetX, y, m_boardOffsetX + boardW, y);
    }
    for (int c = 0; c <= COLS; c++) {
        int x = m_boardOffsetX + c * m_cellSize;
        p.drawLine(x, m_boardOffsetY, x, m_boardOffsetY + boardH);
    }

    // Locked blocks
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (m_field[r][c] >= 0) {
                QRect cr = cellRect(r, c).adjusted(1, 1, -1, -1);
                p.setPen(Qt::NoPen);
                p.setBrush(COLORS[m_field[r][c]]);
                p.drawRoundedRect(cr, 3, 3);
                p.setPen(QPen(COLORS[m_field[r][c]].lighter(140), 1));
                p.drawLine(cr.topLeft() + QPoint(1, 1), cr.topRight() + QPoint(-1, 1));
                p.drawLine(cr.topLeft() + QPoint(1, 1), cr.bottomLeft() + QPoint(1, -1));
            }
        }
    }

    // Current piece
    if (!m_gameOver) {
        auto blocks = m_current.blocks();
        for (const auto &b : blocks) {
            int x = m_current.pos.x() + b.x();
            int y = m_current.pos.y() + b.y();
            if (y >= 0) {
                QRect cr = cellRect(y, x).adjusted(1, 1, -1, -1);
                p.setPen(Qt::NoPen);
                p.setBrush(COLORS[m_current.type]);
                p.drawRoundedRect(cr, 3, 3);
                p.setPen(QPen(COLORS[m_current.type].lighter(150), 1));
                p.drawLine(cr.topLeft() + QPoint(1, 1), cr.topRight() + QPoint(-1, 1));
                p.drawLine(cr.topLeft() + QPoint(1, 1), cr.bottomLeft() + QPoint(1, -1));
            }
        }

        // Ghost piece (drop preview)
        Piece ghost = m_current;
        while (canMove(ghost, 0, 1)) ghost.pos.ry()++;
        if (ghost.pos.y() != m_current.pos.y()) {
            auto gBlocks = ghost.blocks();
            for (const auto &b : gBlocks) {
                int x = ghost.pos.x() + b.x();
                int y = ghost.pos.y() + b.y();
                if (y >= 0) {
                    QRect cr = cellRect(y, x).adjusted(2, 2, -2, -2);
                    p.setPen(QPen(COLORS[m_current.type], 1, Qt::DashLine));
                    p.setBrush(Qt::NoBrush);
                    p.drawRoundedRect(cr, 2, 2);
                }
            }
        }
    }

    // Next piece preview
    int previewX = m_boardOffsetX + boardW + 32;
    int previewY = m_boardOffsetY + 20;

    p.setPen(QColor("#94A3B8"));
    QFont labelFont;
    labelFont.setPixelSize(14);
    labelFont.setBold(true);
    p.setFont(labelFont);
    p.drawText(previewX, previewY - 8, "下一个");

    p.setPen(QColor("#475569"));
    p.setBrush(QColor("#1E293B"));
    p.drawRoundedRect(previewX - 4, previewY, m_cellSize * 5, m_cellSize * 5, 6, 6);

    auto nextBlocks = m_next.blocks();
    for (const auto &b : nextBlocks) {
        QRect cr = previewCellRect(b.y(), b.x(), previewX + m_cellSize / 2, previewY + m_cellSize / 2);
        cr = cr.adjusted(1, 1, -1, -1);
        p.setPen(Qt::NoPen);
        p.setBrush(COLORS[m_next.type]);
        p.drawRoundedRect(cr, 3, 3);
    }

    // Keyboard hint box
    if (!m_gameOver) {
        QString hint = "  ← A  → D  移动    ↑ W  旋转    ↓ S  加速    空格  硬降    P  暂停  ";
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
        p.setPen(QColor("#A855F7"));
        p.drawText(hx, hy + 19, hint);
    }

    if (m_gameOver) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(15, 23, 42, 180));
        p.drawRect(rect());

        p.setPen(QColor("#EF4444"));
        QFont goFont;
        goFont.setPixelSize(36);
        goFont.setBold(true);
        p.setFont(goFont);
        p.drawText(rect(), Qt::AlignCenter, "游戏结束");
    }
}

void TetrisGame::keyPressEvent(QKeyEvent *event)
{
    if (m_gameOver) return;

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_A:
        if (canMove(m_current, -1, 0)) {
            m_current.pos.rx() -= 1;
            update();
        }
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (canMove(m_current, 1, 0)) {
            m_current.pos.rx() += 1;
            update();
        }
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (canMove(m_current, 0, 1)) {
            m_current.pos.ry() += 1;
            m_score += 1;
            m_scoreLabel->setText(QString("得分: %1").arg(m_score));
            update();
        }
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        if (canRotate(m_current)) {
            m_current.rotation = (m_current.rotation + 1) % 4;
            update();
        }
        break;
    case Qt::Key_Space:
        while (canMove(m_current, 0, 1)) {
            m_current.pos.ry() += 1;
            m_score += 2;
        }
        m_scoreLabel->setText(QString("得分: %1").arg(m_score));
        tick();
        break;
    case Qt::Key_P:
        if (m_paused) resumeGame();
        else pauseGame();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void TetrisGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

QRect TetrisGame::cellRect(int row, int col) const
{
    return QRect(m_boardOffsetX + col * m_cellSize,
                 m_boardOffsetY + row * m_cellSize,
                 m_cellSize, m_cellSize);
}

QRect TetrisGame::previewCellRect(int row, int col, int ox, int oy) const
{
    return QRect(ox + col * m_cellSize,
                 oy + row * m_cellSize,
                 m_cellSize, m_cellSize);
}
