// -*- coding: utf-8 -*-
#ifndef DIFFICULTYSELECTWIDGET_H
#define DIFFICULTYSELECTWIDGET_H

#include <QWidget>
#include <QLabel>
#include "gamebase.h"

class QPushButton;

/**
 * DifficultySelectWidget — 难度选择界面
 * 显示简单/中级/高级/自定义/历史记录按钮
 */
class DifficultySelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DifficultySelectWidget(QWidget *parent = nullptr);
    void setGame(const QString &gameName);

signals:
    void difficultyChosen(const QString &gameName, GameBase::Difficulty diff,
                          const QVariantMap &customParams);
    void backRequested();
    void historyRequested(const QString &gameName);

private:
    void setupUi();
    void showCustomDialog();

    QString m_gameName;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_descLabel = nullptr;

    QPushButton *m_easyBtn   = nullptr;
    QPushButton *m_mediumBtn = nullptr;
    QPushButton *m_hardBtn   = nullptr;
    QPushButton *m_customBtn = nullptr;
    QPushButton *m_startBtn  = nullptr;
};

#endif // DIFFICULTYSELECTWIDGET_H
