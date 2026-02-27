// -*- coding: utf-8 -*-
#ifndef GAMESELECTWIDGET_H
#define GAMESELECTWIDGET_H

#include <QWidget>

/**
 * GameSelectWidget — 游戏选择主界面
 * 水平布局：左侧控制面板(文字按钮) + 右侧游戏图片网格
 */
class GameSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameSelectWidget(QWidget *parent = nullptr);

signals:
    void gameSelected(const QString &gameName);
    void historyRequested();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUi();
    QWidget *createSidePanel();
    QWidget *createGameGrid();
    QWidget *createGameCard(const QString &gameName,
                            const QString &displayName,
                            const QString &iconPath);
};

#endif // GAMESELECTWIDGET_H
