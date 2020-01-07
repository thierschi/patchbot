/********************************************************************************
** Form generated from reading UI file 'pb_main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PB_MAIN_WINDOW_H
#define UI_PB_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_main_window
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGroupBox *map_group;
    QGridLayout *gridLayout;
    QLabel *colonie_label;
    QGridLayout *map;
    QScrollBar *map_v_scroll;
    QLabel *map_placeholder;
    QScrollBar *map_h_scroll;
    QGroupBox *controls_box;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *change_colonie;
    QSpacerItem *h_spacer;
    QPushButton *change_colonie_button;
    QGroupBox *program;
    QLabel *program_label;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *prg_contorl;
    QComboBox *prg_control_dropdown;
    QSpacerItem *verticalSpacer;
    QLineEdit *prg_control_edit;
    QScrollBar *prg_control_h_scroll;
    QWidget *gridLayoutWidget_2;
    QGridLayout *prg_arrow_btns;
    QPushButton *prg_arrow_left_btn;
    QPushButton *prg_arrow_down_btn;
    QPushButton *prg_arrow_right_btn;
    QPushButton *prg_arrow_center_btn;
    QPushButton *prg_arrow_back_btn;
    QPushButton *prg_arrow_up_btn;
    QGroupBox *mission_control;
    QLabel *m_c_label;
    QWidget *gridLayoutWidget_3;
    QGridLayout *m_c_btns;
    QPushButton *m_c_start_btn;
    QPushButton *m_c_auto_btn;
    QPushButton *m_c_step_btn;
    QPushButton *m_c_cancel_btn;
    QPushButton *m_c_stop_btn;

    void setupUi(QMainWindow *main_window)
    {
        if (main_window->objectName().isEmpty())
            main_window->setObjectName(QString::fromUtf8("main_window"));
        main_window->resize(820, 550);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(main_window->sizePolicy().hasHeightForWidth());
        main_window->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(main_window);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        map_group = new QGroupBox(centralwidget);
        map_group->setObjectName(QString::fromUtf8("map_group"));
        map_group->setStyleSheet(QString::fromUtf8("QGroupBox#map_group {\n"
"	border: 0px;	\n"
"}"));
        gridLayout = new QGridLayout(map_group);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout->setContentsMargins(-1, -1, -1, 1);
        colonie_label = new QLabel(map_group);
        colonie_label->setObjectName(QString::fromUtf8("colonie_label"));
        colonie_label->setMaximumSize(QSize(16777215, 15));
        colonie_label->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        gridLayout->addWidget(colonie_label, 0, 0, 1, 1);

        map = new QGridLayout();
        map->setSpacing(0);
        map->setObjectName(QString::fromUtf8("map"));
        map->setSizeConstraint(QLayout::SetDefaultConstraint);
        map_v_scroll = new QScrollBar(map_group);
        map_v_scroll->setObjectName(QString::fromUtf8("map_v_scroll"));
        map_v_scroll->setOrientation(Qt::Vertical);

        map->addWidget(map_v_scroll, 0, 1, 1, 1);

        map_placeholder = new QLabel(map_group);
        map_placeholder->setObjectName(QString::fromUtf8("map_placeholder"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(map_placeholder->sizePolicy().hasHeightForWidth());
        map_placeholder->setSizePolicy(sizePolicy1);
        map_placeholder->setStyleSheet(QString::fromUtf8("background: white;\n"
""));
        map_placeholder->setAlignment(Qt::AlignCenter);
        map_placeholder->setMargin(-5);

        map->addWidget(map_placeholder, 0, 0, 1, 1);

        map_h_scroll = new QScrollBar(map_group);
        map_h_scroll->setObjectName(QString::fromUtf8("map_h_scroll"));
        map_h_scroll->setOrientation(Qt::Horizontal);

        map->addWidget(map_h_scroll, 1, 0, 1, 1);


        gridLayout->addLayout(map, 1, 0, 1, 1);


        gridLayout_2->addWidget(map_group, 0, 0, 1, 1);

        controls_box = new QGroupBox(centralwidget);
        controls_box->setObjectName(QString::fromUtf8("controls_box"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(controls_box->sizePolicy().hasHeightForWidth());
        controls_box->setSizePolicy(sizePolicy2);
        controls_box->setMinimumSize(QSize(231, 471));
        controls_box->setStyleSheet(QString::fromUtf8("QGroupBox#controls_box\n"
" {\n"
"	border: 0px;\n"
"}"));
        controls_box->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        horizontalLayoutWidget = new QWidget(controls_box);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 231, 27));
        change_colonie = new QHBoxLayout(horizontalLayoutWidget);
        change_colonie->setSpacing(0);
        change_colonie->setObjectName(QString::fromUtf8("change_colonie"));
        change_colonie->setContentsMargins(0, 0, 0, 0);
        h_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        change_colonie->addItem(h_spacer);

        change_colonie_button = new QPushButton(horizontalLayoutWidget);
        change_colonie_button->setObjectName(QString::fromUtf8("change_colonie_button"));

        change_colonie->addWidget(change_colonie_button);

        program = new QGroupBox(controls_box);
        program->setObjectName(QString::fromUtf8("program"));
        program->setGeometry(QRect(0, 29, 231, 141));
        program->setStyleSheet(QString::fromUtf8(""));
        program_label = new QLabel(program);
        program_label->setObjectName(QString::fromUtf8("program_label"));
        program_label->setGeometry(QRect(0, 0, 231, 31));
        program_label->setAlignment(Qt::AlignCenter);
        verticalLayoutWidget_2 = new QWidget(program);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(120, 30, 101, 101));
        prg_contorl = new QVBoxLayout(verticalLayoutWidget_2);
        prg_contorl->setSpacing(0);
        prg_contorl->setObjectName(QString::fromUtf8("prg_contorl"));
        prg_contorl->setContentsMargins(0, 2, 0, 0);
        prg_control_dropdown = new QComboBox(verticalLayoutWidget_2);
        prg_control_dropdown->addItem(QString());
        prg_control_dropdown->setObjectName(QString::fromUtf8("prg_control_dropdown"));
        prg_control_dropdown->setMinimumSize(QSize(0, 27));
        prg_control_dropdown->setEditable(false);

        prg_contorl->addWidget(prg_control_dropdown);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        prg_contorl->addItem(verticalSpacer);

        prg_control_edit = new QLineEdit(verticalLayoutWidget_2);
        prg_control_edit->setObjectName(QString::fromUtf8("prg_control_edit"));
        prg_control_edit->setReadOnly(true);

        prg_contorl->addWidget(prg_control_edit);

        prg_control_h_scroll = new QScrollBar(verticalLayoutWidget_2);
        prg_control_h_scroll->setObjectName(QString::fromUtf8("prg_control_h_scroll"));
        prg_control_h_scroll->setOrientation(Qt::Horizontal);

        prg_contorl->addWidget(prg_control_h_scroll);

        gridLayoutWidget_2 = new QWidget(program);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(10, 30, 101, 101));
        prg_arrow_btns = new QGridLayout(gridLayoutWidget_2);
        prg_arrow_btns->setSpacing(4);
        prg_arrow_btns->setObjectName(QString::fromUtf8("prg_arrow_btns"));
        prg_arrow_btns->setSizeConstraint(QLayout::SetDefaultConstraint);
        prg_arrow_btns->setContentsMargins(0, 0, 0, 0);
        prg_arrow_left_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_left_btn->setObjectName(QString::fromUtf8("prg_arrow_left_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_left_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_left_btn->setSizePolicy(sizePolicy);
        prg_arrow_left_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_left_btn->setMaximumSize(QSize(30, 30));
        QFont font;
        font.setPointSize(11);
        prg_arrow_left_btn->setFont(font);

        prg_arrow_btns->addWidget(prg_arrow_left_btn, 1, 0, 1, 1);

        prg_arrow_down_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_down_btn->setObjectName(QString::fromUtf8("prg_arrow_down_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_down_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_down_btn->setSizePolicy(sizePolicy);
        prg_arrow_down_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_down_btn->setMaximumSize(QSize(30, 30));
        prg_arrow_down_btn->setFont(font);

        prg_arrow_btns->addWidget(prg_arrow_down_btn, 2, 1, 1, 1);

        prg_arrow_right_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_right_btn->setObjectName(QString::fromUtf8("prg_arrow_right_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_right_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_right_btn->setSizePolicy(sizePolicy);
        prg_arrow_right_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_right_btn->setMaximumSize(QSize(30, 30));
        prg_arrow_right_btn->setFont(font);

        prg_arrow_btns->addWidget(prg_arrow_right_btn, 1, 2, 1, 1);

        prg_arrow_center_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_center_btn->setObjectName(QString::fromUtf8("prg_arrow_center_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_center_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_center_btn->setSizePolicy(sizePolicy);
        prg_arrow_center_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_center_btn->setMaximumSize(QSize(30, 30));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(false);
        font1.setWeight(50);
        prg_arrow_center_btn->setFont(font1);

        prg_arrow_btns->addWidget(prg_arrow_center_btn, 1, 1, 1, 1);

        prg_arrow_back_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_back_btn->setObjectName(QString::fromUtf8("prg_arrow_back_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_back_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_back_btn->setSizePolicy(sizePolicy);
        prg_arrow_back_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_back_btn->setMaximumSize(QSize(30, 30));
        prg_arrow_back_btn->setFont(font);

        prg_arrow_btns->addWidget(prg_arrow_back_btn, 0, 2, 1, 1);

        prg_arrow_up_btn = new QPushButton(gridLayoutWidget_2);
        prg_arrow_up_btn->setObjectName(QString::fromUtf8("prg_arrow_up_btn"));
        sizePolicy.setHeightForWidth(prg_arrow_up_btn->sizePolicy().hasHeightForWidth());
        prg_arrow_up_btn->setSizePolicy(sizePolicy);
        prg_arrow_up_btn->setMinimumSize(QSize(30, 30));
        prg_arrow_up_btn->setMaximumSize(QSize(30, 30));
        prg_arrow_up_btn->setFont(font);

        prg_arrow_btns->addWidget(prg_arrow_up_btn, 0, 1, 1, 1);

        mission_control = new QGroupBox(controls_box);
        mission_control->setObjectName(QString::fromUtf8("mission_control"));
        mission_control->setGeometry(QRect(0, 180, 231, 131));
        m_c_label = new QLabel(mission_control);
        m_c_label->setObjectName(QString::fromUtf8("m_c_label"));
        m_c_label->setGeometry(QRect(0, 0, 231, 31));
        m_c_label->setAlignment(Qt::AlignCenter);
        gridLayoutWidget_3 = new QWidget(mission_control);
        gridLayoutWidget_3->setObjectName(QString::fromUtf8("gridLayoutWidget_3"));
        gridLayoutWidget_3->setGeometry(QRect(0, 30, 231, 101));
        m_c_btns = new QGridLayout(gridLayoutWidget_3);
        m_c_btns->setObjectName(QString::fromUtf8("m_c_btns"));
        m_c_btns->setHorizontalSpacing(5);
        m_c_btns->setContentsMargins(5, 5, 5, 5);
        m_c_start_btn = new QPushButton(gridLayoutWidget_3);
        m_c_start_btn->setObjectName(QString::fromUtf8("m_c_start_btn"));

        m_c_btns->addWidget(m_c_start_btn, 0, 0, 1, 1);

        m_c_auto_btn = new QPushButton(gridLayoutWidget_3);
        m_c_auto_btn->setObjectName(QString::fromUtf8("m_c_auto_btn"));

        m_c_btns->addWidget(m_c_auto_btn, 1, 1, 1, 1);

        m_c_step_btn = new QPushButton(gridLayoutWidget_3);
        m_c_step_btn->setObjectName(QString::fromUtf8("m_c_step_btn"));

        m_c_btns->addWidget(m_c_step_btn, 1, 0, 1, 1);

        m_c_cancel_btn = new QPushButton(gridLayoutWidget_3);
        m_c_cancel_btn->setObjectName(QString::fromUtf8("m_c_cancel_btn"));

        m_c_btns->addWidget(m_c_cancel_btn, 0, 1, 1, 1);

        m_c_stop_btn = new QPushButton(gridLayoutWidget_3);
        m_c_stop_btn->setObjectName(QString::fromUtf8("m_c_stop_btn"));

        m_c_btns->addWidget(m_c_stop_btn, 2, 1, 1, 1);


        gridLayout_2->addWidget(controls_box, 0, 1, 1, 1);

        main_window->setCentralWidget(centralwidget);

        retranslateUi(main_window);

        QMetaObject::connectSlotsByName(main_window);
    } // setupUi

    void retranslateUi(QMainWindow *main_window)
    {
        main_window->setWindowTitle(QApplication::translate("main_window", " PATCHBOT", nullptr));
        colonie_label->setText(QApplication::translate("main_window", "Aktuelle Kolonie: Lithium-Stollen 13-A", nullptr));
        map_placeholder->setText(QApplication::translate("main_window", "Umgebungskarte", nullptr));
        controls_box->setTitle(QString());
        change_colonie_button->setText(QApplication::translate("main_window", "Andere Kolonie...", nullptr));
        program->setTitle(QString());
        program_label->setText(QApplication::translate("main_window", "Programmieren", nullptr));
        prg_control_dropdown->setItemText(0, QApplication::translate("main_window", "Dropdown", nullptr));

        prg_control_edit->setText(QApplication::translate("main_window", "Textfeld", nullptr));
        prg_arrow_left_btn->setText(QApplication::translate("main_window", "\342\206\220", nullptr));
        prg_arrow_down_btn->setText(QApplication::translate("main_window", "\342\206\223", nullptr));
        prg_arrow_right_btn->setText(QApplication::translate("main_window", "\342\206\222", nullptr));
        prg_arrow_center_btn->setText(QApplication::translate("main_window", "\342\200\242", nullptr));
        prg_arrow_back_btn->setText(QApplication::translate("main_window", "\342\257\207", nullptr));
        prg_arrow_up_btn->setText(QApplication::translate("main_window", "\342\206\221", nullptr));
        mission_control->setTitle(QString());
        m_c_label->setText(QApplication::translate("main_window", "Missionsablauf", nullptr));
        m_c_start_btn->setText(QApplication::translate("main_window", "Start", nullptr));
        m_c_auto_btn->setText(QApplication::translate("main_window", "Automatik", nullptr));
        m_c_step_btn->setText(QApplication::translate("main_window", "Einzelschritt", nullptr));
        m_c_cancel_btn->setText(QApplication::translate("main_window", "Abbruch", nullptr));
        m_c_stop_btn->setText(QApplication::translate("main_window", "Anhalten", nullptr));
    } // retranslateUi

};

namespace Ui {
    class main_window: public Ui_main_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PB_MAIN_WINDOW_H
