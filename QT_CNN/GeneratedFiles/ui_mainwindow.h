/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "glwidget.h"
#include "plotwidget.h"
#include "resultdistribute.h"
#include "selectionlabel.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *CovPlot;
    QGridLayout *gridLayout_3;
    QLabel *C2Result;
    QLabel *C1Result;
    ResultDistribute *ResultPlot;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_2;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QGridLayout *gridLayout_5;
    SelectionLabel *test_mat;
    QWidget *page_2;
    QGridLayout *gridLayout_4;
    QLabel *label_7;
    GLWidget *openGLWidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLabel *CrossEntropy;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QProgressBar *TestAccuracy;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *kernel1_mat;
    QSpacerItem *verticalSpacer;
    QLabel *kernel2_mat;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QProgressBar *TrainAccuracy;
    PlotWidget *EntropyPlot;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(578, 389);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_6 = new QGridLayout(centralWidget);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        CovPlot = new QGroupBox(centralWidget);
        CovPlot->setObjectName(QStringLiteral("CovPlot"));
        CovPlot->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(CovPlot->sizePolicy().hasHeightForWidth());
        CovPlot->setSizePolicy(sizePolicy1);
        CovPlot->setMouseTracking(false);
        gridLayout_3 = new QGridLayout(CovPlot);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        C2Result = new QLabel(CovPlot);
        C2Result->setObjectName(QStringLiteral("C2Result"));

        gridLayout_3->addWidget(C2Result, 3, 0, 1, 1, Qt::AlignHCenter);

        C1Result = new QLabel(CovPlot);
        C1Result->setObjectName(QStringLiteral("C1Result"));

        gridLayout_3->addWidget(C1Result, 1, 0, 1, 1, Qt::AlignHCenter);

        ResultPlot = new ResultDistribute(CovPlot);
        ResultPlot->setObjectName(QStringLiteral("ResultPlot"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ResultPlot->sizePolicy().hasHeightForWidth());
        ResultPlot->setSizePolicy(sizePolicy2);
        ResultPlot->setMinimumSize(QSize(50, 50));

        gridLayout_3->addWidget(ResultPlot, 5, 0, 1, 1);

        label_5 = new QLabel(CovPlot);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_3->addWidget(label_5, 0, 0, 1, 1);

        label_6 = new QLabel(CovPlot);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 2, 0, 1, 1);

        label_2 = new QLabel(CovPlot);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 4, 0, 1, 1);


        horizontalLayout_4->addWidget(CovPlot);

        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        gridLayout_5 = new QGridLayout(page);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        test_mat = new SelectionLabel(page);
        test_mat->setObjectName(QStringLiteral("test_mat"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(test_mat->sizePolicy().hasHeightForWidth());
        test_mat->setSizePolicy(sizePolicy3);

        gridLayout_5->addWidget(test_mat, 0, 0, 1, 1);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        gridLayout_4 = new QGridLayout(page_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_7 = new QLabel(page_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_4->addWidget(label_7, 0, 0, 1, 1);

        openGLWidget = new GLWidget(page_2);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy4);
        openGLWidget->setMinimumSize(QSize(100, 100));

        gridLayout_4->addWidget(openGLWidget, 1, 0, 1, 1);

        stackedWidget->addWidget(page_2);

        horizontalLayout_4->addWidget(stackedWidget);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        CrossEntropy = new QLabel(centralWidget);
        CrossEntropy->setObjectName(QStringLiteral("CrossEntropy"));

        horizontalLayout_3->addWidget(CrossEntropy);


        gridLayout_2->addLayout(horizontalLayout_3, 4, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        TestAccuracy = new QProgressBar(centralWidget);
        TestAccuracy->setObjectName(QStringLiteral("TestAccuracy"));
        sizePolicy.setHeightForWidth(TestAccuracy->sizePolicy().hasHeightForWidth());
        TestAccuracy->setSizePolicy(sizePolicy);
        TestAccuracy->setValue(24);

        horizontalLayout->addWidget(TestAccuracy);


        gridLayout_2->addLayout(horizontalLayout, 1, 0, 1, 1);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        kernel1_mat = new QLabel(groupBox);
        kernel1_mat->setObjectName(QStringLiteral("kernel1_mat"));

        gridLayout->addWidget(kernel1_mat, 0, 0, 1, 1, Qt::AlignHCenter);

        verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);

        kernel2_mat = new QLabel(groupBox);
        kernel2_mat->setObjectName(QStringLiteral("kernel2_mat"));

        gridLayout->addWidget(kernel2_mat, 2, 0, 1, 1, Qt::AlignHCenter);

        kernel2_mat->raise();
        kernel1_mat->raise();

        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        TrainAccuracy = new QProgressBar(centralWidget);
        TrainAccuracy->setObjectName(QStringLiteral("TrainAccuracy"));
        sizePolicy.setHeightForWidth(TrainAccuracy->sizePolicy().hasHeightForWidth());
        TrainAccuracy->setSizePolicy(sizePolicy);
        TrainAccuracy->setValue(24);

        horizontalLayout_2->addWidget(TrainAccuracy);


        gridLayout_2->addLayout(horizontalLayout_2, 2, 0, 1, 1);

        EntropyPlot = new PlotWidget(centralWidget);
        EntropyPlot->setObjectName(QStringLiteral("EntropyPlot"));
        sizePolicy2.setHeightForWidth(EntropyPlot->sizePolicy().hasHeightForWidth());
        EntropyPlot->setSizePolicy(sizePolicy2);
        EntropyPlot->setMinimumSize(QSize(100, 100));

        gridLayout_2->addWidget(EntropyPlot, 3, 0, 1, 1);


        horizontalLayout_4->addLayout(gridLayout_2);


        gridLayout_6->addLayout(horizontalLayout_4, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        stackedWidget->raise();
        EntropyPlot->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 578, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        CovPlot->setTitle(QApplication::translate("MainWindow", "\345\215\267\347\247\257\346\203\205\345\206\265", nullptr));
        C2Result->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        C1Result->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "\347\254\254\344\270\200\345\261\202\345\215\267\347\247\257\357\274\232", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "\347\254\254\344\272\214\345\261\202\345\215\267\347\247\257\357\274\232", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\351\242\204\346\265\213\346\246\202\347\216\207\357\274\232", nullptr));
        test_mat->setText(QApplication::translate("MainWindow", "test_mat", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\345\215\267\347\247\257\347\245\236\347\273\217\347\275\221\347\273\234\357\274\232", nullptr));
#ifndef QT_NO_STATUSTIP
        label_4->setStatusTip(QApplication::translate("MainWindow", "\344\272\244\345\217\211\347\206\265", nullptr));
#endif // QT_NO_STATUSTIP
        label_4->setText(QApplication::translate("MainWindow", "\344\272\244\345\217\211\347\206\265\357\274\232", nullptr));
        CrossEntropy->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
#ifndef QT_NO_STATUSTIP
        label->setStatusTip(QApplication::translate("MainWindow", "\346\265\213\350\257\225\351\233\206\345\207\206\347\241\256\347\216\207", nullptr));
#endif // QT_NO_STATUSTIP
        label->setText(QApplication::translate("MainWindow", "\346\265\213\350\257\225\351\233\206\345\207\206\347\241\256\347\216\207\357\274\232", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "\345\215\267\347\247\257\346\240\270", nullptr));
#ifndef QT_NO_STATUSTIP
        kernel1_mat->setStatusTip(QApplication::translate("MainWindow", "\347\254\254\344\270\200\345\261\202\345\215\267\347\247\257\346\240\270", nullptr));
#endif // QT_NO_STATUSTIP
        kernel1_mat->setText(QApplication::translate("MainWindow", "kernel1_mat", nullptr));
#ifndef QT_NO_TOOLTIP
        kernel2_mat->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        kernel2_mat->setStatusTip(QApplication::translate("MainWindow", "\347\254\254\344\272\214\345\261\202\345\215\267\347\247\257\346\240\270", nullptr));
#endif // QT_NO_STATUSTIP
        kernel2_mat->setText(QApplication::translate("MainWindow", "kernel2_mat", nullptr));
#ifndef QT_NO_STATUSTIP
        label_3->setStatusTip(QApplication::translate("MainWindow", "\350\256\255\347\273\203\351\233\206\345\207\206\347\241\256\347\216\207", nullptr));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        label_3->setWhatsThis(QApplication::translate("MainWindow", "<html><head/><body><p>\350\256\255\347\273\203\351\233\206\345\207\206\347\241\256\347\216\207</p></body></html>", nullptr));
#endif // QT_NO_WHATSTHIS
        label_3->setText(QApplication::translate("MainWindow", "\350\256\255\347\273\203\351\233\206\345\207\206\347\241\256\347\216\207\357\274\232", nullptr));
#ifndef QT_NO_TOOLTIP
        TrainAccuracy->setToolTip(QApplication::translate("MainWindow", "\350\256\255\347\273\203\351\233\206", nullptr));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
