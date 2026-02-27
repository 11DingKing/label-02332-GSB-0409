// -*- coding: utf-8 -*-
#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QStackedWidget>

/**
 * HistoryWidget — 历史记录查看界面
 * 两个子页面：游戏选择列表 + 具体游戏记录表格
 */
class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    void loadRecords(const QString &gameName = "");

signals:
    void backRequested();

private:
    void setupUi();
    QWidget *createGameListPage();
    QWidget *createRecordTablePage();
    void showGameRecords(const QString &gameName);
    void populateTable(const QString &gameName);

    QStackedWidget *m_innerStack = nullptr;
    QTableWidget *m_table = nullptr;
    QLabel *m_recordTitle = nullptr;
    QString m_currentGame;
};

#endif // HISTORYWIDGET_H
