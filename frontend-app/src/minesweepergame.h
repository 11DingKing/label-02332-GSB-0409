// -*- coding: utf-8 -*-
#ifndef MINESWEEPERGAME_H
#define MINESWEEPERGAME_H

#include "gamebase.h"
#include <QVector>
#include <QTimer>
#include <QLabel>
#include <QPoint>

/**
 * MinesweeperGame — 扫雷
 * 支持左键揭开、右键标旗，自动展开空白区域
 */
class MinesweeperGame : public GameBase
{
    Q_OBJECT

public:
    explicit MinesweeperGame(QWidget *parent = nullptr);

    void pauseGame() override;
    void resumeGame() override;

protected:
    void initGame() override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum CellState { Hidden, Revealed, Flagged };

    struct Cell {
        bool hasMine = false;
        int adjacentMines = 0;
        CellState state = Hidden;
    };

    void generateMines(int firstRow, int firstCol);
    void revealCell(int row, int col);
    void revealAllMines();
    void checkWinCondition();
    int countAdjacentMines(int row, int col) const;
    QRect cellRect(int row, int col) const;
    QPoint cellFromPos(const QPoint &pos) const;

    int m_rows = 9;
    int m_cols = 9;
    int m_mineCount = 10;
    int m_flagCount = 0;
    int m_revealedCount = 0;
    bool m_firstClick = true;
    bool m_gameOver = false;

    QVector<QVector<Cell>> m_grid;
    QTimer *m_timer = nullptr;
    int m_elapsedSeconds = 0;

    QLabel *m_timeLabel = nullptr;
    QLabel *m_mineLabel = nullptr;

    int m_cellSize = 32;
    int m_boardOffsetX = 0;
    int m_boardOffsetY = 0;
};

#endif // MINESWEEPERGAME_H
