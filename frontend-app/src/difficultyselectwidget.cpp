// -*- coding: utf-8 -*-
#include "difficultyselectwidget.h"
#include "historymanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>

DifficultySelectWidget::DifficultySelectWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void DifficultySelectWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(60, 48, 60, 48);
    mainLayout->setAlignment(Qt::AlignCenter);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &DifficultySelectWidget::backRequested);

    auto *topBar = new QHBoxLayout();
    topBar->addWidget(backBtn);
    topBar->addStretch();
    mainLayout->addLayout(topBar);

    mainLayout->addSpacing(32);

    m_titleLabel = new QLabel("选择难度", this);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_titleLabel);

    m_descLabel = new QLabel("", this);
    m_descLabel->setObjectName("subtitleLabel");
    m_descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_descLabel);

    mainLayout->addSpacing(40);

    auto *btnContainer = new QWidget(this);
    auto *btnLayout = new QVBoxLayout(btnContainer);
    btnLayout->setSpacing(16);
    btnLayout->setAlignment(Qt::AlignCenter);

    struct DiffInfo {
        QString label;
        GameBase::Difficulty diff;
        QString style;
    };

    const QList<DiffInfo> diffs = {
        {"简  单", GameBase::Easy,
         "QPushButton { background-color: #10B981; border-color: #34D399; color: white; font-size: 18px; font-weight: 600; }"
         "QPushButton:hover { background-color: #34D399; }"},
        {"中  级", GameBase::Medium,
         "QPushButton { background-color: #F59E0B; border-color: #FBBF24; color: white; font-size: 18px; font-weight: 600; }"
         "QPushButton:hover { background-color: #FBBF24; }"},
        {"高  级", GameBase::Hard,
         "QPushButton { background-color: #EF4444; border-color: #F87171; color: white; font-size: 18px; font-weight: 600; }"
         "QPushButton:hover { background-color: #F87171; }"},
    };

    QPushButton **diffBtns[] = {&m_easyBtn, &m_mediumBtn, &m_hardBtn};
    for (int i = 0; i < 3; ++i) {
        auto *btn = new QPushButton(diffs[i].label, btnContainer);
        btn->setFixedSize(280, 52);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(diffs[i].style + " QPushButton { border-radius: 10px; }");
        connect(btn, &QPushButton::clicked, this, [this, diff = diffs[i].diff]() {
            emit difficultyChosen(m_gameName, diff, {});
        });
        btnLayout->addWidget(btn, 0, Qt::AlignCenter);
        *diffBtns[i] = btn;
    }

    m_customBtn = new QPushButton("自定义", btnContainer);
    m_customBtn->setFixedSize(280, 52);
    m_customBtn->setCursor(Qt::PointingHandCursor);
    m_customBtn->setStyleSheet(
        "QPushButton { background-color: #8B5CF6; border-color: #A78BFA; color: white; "
        "font-size: 18px; font-weight: 600; border-radius: 10px; }"
        "QPushButton:hover { background-color: #A78BFA; }"
    );
    connect(m_customBtn, &QPushButton::clicked, this, &DifficultySelectWidget::showCustomDialog);
    btnLayout->addWidget(m_customBtn, 0, Qt::AlignCenter);

    // 立即开始 — only shown for games without difficulty (gomoku / reversi)
    m_startBtn = new QPushButton("开  始  游  戏", btnContainer);
    m_startBtn->setFixedSize(280, 60);
    m_startBtn->setCursor(Qt::PointingHandCursor);
    m_startBtn->setStyleSheet(
        "QPushButton { background-color: #6366F1; border-color: #818CF8; color: white; "
        "font-size: 20px; font-weight: 700; border-radius: 10px; }"
        "QPushButton:hover { background-color: #818CF8; }"
    );
    connect(m_startBtn, &QPushButton::clicked, this, [this]() {
        emit difficultyChosen(m_gameName, GameBase::Easy, {});
    });
    btnLayout->addWidget(m_startBtn, 0, Qt::AlignCenter);

    btnLayout->addSpacing(8);

    auto *histBtn = new QPushButton("历史记录", btnContainer);
    histBtn->setFixedSize(280, 52);
    histBtn->setCursor(Qt::PointingHandCursor);
    histBtn->setObjectName("accentBtn");
    histBtn->setStyleSheet(
        "QPushButton { background-color: #0E7490; border-color: #22D3EE; color: white; "
        "font-size: 18px; font-weight: 600; border-radius: 10px; }"
        "QPushButton:hover { background-color: #0891B2; }"
    );
    connect(histBtn, &QPushButton::clicked, this, [this]() {
        emit historyRequested(m_gameName);
    });
    btnLayout->addWidget(histBtn, 0, Qt::AlignCenter);

    mainLayout->addWidget(btnContainer);
    mainLayout->addStretch();
}

void DifficultySelectWidget::setGame(const QString &gameName)
{
    m_gameName = gameName;
    m_titleLabel->setText(HistoryManager::gameDisplayName(gameName));

    bool hasDiff = (gameName != "gomoku" && gameName != "reversi");
    m_easyBtn->setVisible(hasDiff);
    m_mediumBtn->setVisible(hasDiff);
    m_hardBtn->setVisible(hasDiff);
    m_customBtn->setVisible(hasDiff);
    m_startBtn->setVisible(!hasDiff);

    if (hasDiff) {
        m_descLabel->setText("选择难度开始游戏");
    } else {
        m_descLabel->setText("双人对战");
    }
}

void DifficultySelectWidget::showCustomDialog()
{
    auto *dialog = new QDialog(this);
    dialog->setWindowTitle("自定义难度");
    dialog->setMinimumWidth(360);
    dialog->setStyleSheet(
        "QDialog { background-color: #1E293B; border: 1px solid #475569; }"
        "QLabel { color: #F1F5F9; font-size: 14px; }"
        "QSpinBox { background-color: #334155; border: 1px solid #475569; border-radius: 6px; "
        "padding: 4px 8px; color: #F1F5F9; min-height: 28px; font-size: 14px; }"
        "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; "
        "width: 20px; border-left: 1px solid #475569; background: #3B4D6B; border-top-right-radius: 6px; }"
        "QSpinBox::up-button:hover { background: #6366F1; }"
        "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; "
        "width: 20px; border-left: 1px solid #475569; background: #3B4D6B; border-bottom-right-radius: 6px; }"
        "QSpinBox::down-button:hover { background: #6366F1; }"
        "QSpinBox::up-arrow { width: 8px; height: 8px; }"
        "QSpinBox::down-arrow { width: 8px; height: 8px; }"
    );

    auto *formLayout = new QFormLayout(dialog);
    formLayout->setContentsMargins(24, 24, 24, 24);
    formLayout->setSpacing(16);

    QMap<QString, QSpinBox*> spinBoxes;

    if (m_gameName == "minesweeper") {
        auto *rows = new QSpinBox(dialog); rows->setRange(5, 30); rows->setValue(9);
        auto *cols = new QSpinBox(dialog); cols->setRange(5, 30); cols->setValue(9);
        auto *mines = new QSpinBox(dialog); mines->setRange(1, 200); mines->setValue(10);
        formLayout->addRow("行数:", rows);
        formLayout->addRow("列数:", cols);
        formLayout->addRow("地雷数:", mines);
        spinBoxes["rows"] = rows;
        spinBoxes["cols"] = cols;
        spinBoxes["mines"] = mines;
    } else if (m_gameName == "snake") {
        auto *speed = new QSpinBox(dialog); speed->setRange(50, 500); speed->setValue(150);
        speed->setSuffix(" ms");
        formLayout->addRow("移动间隔:", speed);
        spinBoxes["speed"] = speed;
    } else if (m_gameName == "gomoku") {
        auto *boardSize = new QSpinBox(dialog); boardSize->setRange(9, 19); boardSize->setValue(15);
        formLayout->addRow("棋盘大小:", boardSize);
        spinBoxes["board_size"] = boardSize;
    } else if (m_gameName == "reversi") {
        auto *infoLabel = new QLabel("黑白棋固定为 8×8 棋盘", dialog);
        infoLabel->setAlignment(Qt::AlignCenter);
        formLayout->addRow(infoLabel);
    } else if (m_gameName == "tetris") {
        auto *startLevel = new QSpinBox(dialog); startLevel->setRange(1, 15); startLevel->setValue(1);
        formLayout->addRow("起始等级:", startLevel);
        spinBoxes["start_level"] = startLevel;
    }

    auto *buttonBox = new QDialogButtonBox(dialog);
    auto *okBtn = buttonBox->addButton("开始游戏", QDialogButtonBox::AcceptRole);
    auto *cancelBtn = buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
    okBtn->setStyleSheet(
        "QPushButton { background-color: #6366F1; color: white; border: none; "
        "border-radius: 6px; padding: 8px 24px; font-size: 14px; }"
        "QPushButton:hover { background-color: #818CF8; }"
    );
    cancelBtn->setStyleSheet(
        "QPushButton { background-color: #334155; color: #F1F5F9; border: 1px solid #475569; "
        "border-radius: 6px; padding: 8px 24px; font-size: 14px; }"
        "QPushButton:hover { background-color: #475569; }"
    );
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted) {
        QVariantMap params;
        for (auto it = spinBoxes.begin(); it != spinBoxes.end(); ++it) {
            params[it.key()] = it.value()->value();
        }
        emit difficultyChosen(m_gameName, GameBase::Custom, params);
    }

    dialog->deleteLater();
}
