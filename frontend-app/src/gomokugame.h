// -*- coding: utf-8 -*-
#ifndef GOMOKUGAME_H
#define GOMOKUGAME_H

#include "gamebase.h"
#include <QVector>
#include <QLabel>
#include <QPoint>

/**
 * GomokuGame — 五子棋（双人对战）
 * 15×15（或自定义）棋盘，黑白交替落子，五子连珠获胜
 */
class GomokuGame : public GameBase
{
    Q_OBJECT

public:
    explicit GomokuGame(QWidget *parent = nullptr);

protected:
    void initGame() override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum Stone { Empty, Black, White };

    bool checkWin(int row, int col, Stone stone) const;
    int countDirection(int row, int col, int dr, int dc, Stone stone) const;
    QPoint nearestIntersection(const QPoint &pos) const;
    QPointF intersectionCenter(int row, int col) const;

    int m_boardSize = 15;
    QVector<QVector<Stone>> m_board;
    Stone m_currentPlayer = Black;
    bool m_gameOver = false;
    QPoint m_lastMove{-1, -1};

    QLabel *m_statusLabel = nullptr;

    double m_gridSize = 0;
    double m_boardOffsetX = 0;
    double m_boardOffsetY = 0;
    double m_margin = 30;
};

#endif // GOMOKUGAME_H
