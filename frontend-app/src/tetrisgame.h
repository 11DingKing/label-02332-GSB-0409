// -*- coding: utf-8 -*-
#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "gamebase.h"
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QPoint>
#include <QColor>

/**
 * TetrisGame — 俄罗斯方块
 * 标准 7 种方块（I/O/T/S/Z/J/L），SRS 简化旋转，消行计分
 */
class TetrisGame : public GameBase
{
    Q_OBJECT

public:
    explicit TetrisGame(QWidget *parent = nullptr);

    void pauseGame() override;
    void resumeGame() override;

protected:
    void initGame() override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    static constexpr int ROWS = 20;
    static constexpr int COLS = 10;

    struct Piece {
        int type;
        int rotation;
        QPoint pos;
        QVector<QPoint> blocks() const;
    };

    void tick();
    void spawnPiece();
    bool canMove(const Piece &piece, int dx, int dy) const;
    bool canRotate(const Piece &piece) const;
    void lockPiece();
    int clearLines();
    void updateLevel();
    QRect cellRect(int row, int col) const;
    QRect previewCellRect(int row, int col, int ox, int oy) const;

    static const QVector<QVector<QVector<QPoint>>> SHAPES;
    static const QColor COLORS[7];

    QVector<QVector<int>> m_field;
    Piece m_current;
    Piece m_next;
    bool m_gameOver = false;

    int m_score = 0;
    int m_level = 1;
    int m_linesCleared = 0;

    QTimer *m_dropTimer = nullptr;
    int m_baseSpeed = 500;

    QLabel *m_scoreLabel = nullptr;
    QLabel *m_levelLabel = nullptr;
    QLabel *m_linesLabel = nullptr;

    int m_cellSize = 28;
    int m_boardOffsetX = 0;
    int m_boardOffsetY = 0;
};

#endif // TETRISGAME_H
