# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 5.15.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import (QCoreApplication, QMetaObject,
                            QRect, QSize, Qt)
from PySide2.QtGui import (QFont)
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(960, 640)
        self.newDocAction = QAction(MainWindow)
        self.newDocAction.setObjectName(u"newDocAction")
        self.newProAction = QAction(MainWindow)
        self.newProAction.setObjectName(u"newProAction")
        self.openProAction = QAction(MainWindow)
        self.openProAction.setObjectName(u"openProAction")
        self.recentAction = QAction(MainWindow)
        self.recentAction.setObjectName(u"recentAction")
        self.saveAllAction = QAction(MainWindow)
        self.saveAllAction.setObjectName(u"saveAllAction")
        self.closeAllAction = QAction(MainWindow)
        self.closeAllAction.setObjectName(u"closeAllAction")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.gridLayout_2 = QGridLayout(self.centralwidget)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.gridLayout = QGridLayout()
        self.gridLayout.setSpacing(5)
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(2, 2, 2, 2)
        self.mdiArea = QMdiArea(self.centralwidget)
        self.mdiArea.setObjectName(u"mdiArea")
        self.mdiArea.setViewMode(QMdiArea.TabbedView)
        self.mdiArea.setDocumentMode(True)
        self.mdiArea.setTabsClosable(True)
        self.mdiArea.setTabsMovable(True)

        self.gridLayout.addWidget(self.mdiArea, 1, 1, 1, 1)

        self.buttomLayout = QVBoxLayout()
        self.buttomLayout.setSpacing(4)
        self.buttomLayout.setObjectName(u"buttomLayout")
        self.buttomLayout.setContentsMargins(2, 2, 2, 2)
        self.buttomButtoNlAYOUT = QHBoxLayout()
        self.buttomButtoNlAYOUT.setSpacing(20)
        self.buttomButtoNlAYOUT.setObjectName(u"buttomButtoNlAYOUT")
        self.buttomButtoNlAYOUT.setContentsMargins(2, 2, 2, 2)
        self.emptyBrowserButton = QToolButton(self.centralwidget)
        self.emptyBrowserButton.setObjectName(u"emptyBrowserButton")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.emptyBrowserButton.sizePolicy().hasHeightForWidth())
        self.emptyBrowserButton.setSizePolicy(sizePolicy)
        font = QFont()
        font.setFamily(u"Arial")
        font.setPointSize(12)
        font.setBold(True)
        font.setUnderline(False)
        font.setWeight(75)
        self.emptyBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.emptyBrowserButton)

        self.leftTextBrowserButton = QToolButton(self.centralwidget)
        self.leftTextBrowserButton.setObjectName(u"leftTextBrowserButton")
        sizePolicy.setHeightForWidth(self.leftTextBrowserButton.sizePolicy().hasHeightForWidth())
        self.leftTextBrowserButton.setSizePolicy(sizePolicy)
        self.leftTextBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.leftTextBrowserButton)

        self.rightTextBrowserButton = QToolButton(self.centralwidget)
        self.rightTextBrowserButton.setObjectName(u"rightTextBrowserButton")
        sizePolicy.setHeightForWidth(self.rightTextBrowserButton.sizePolicy().hasHeightForWidth())
        self.rightTextBrowserButton.setSizePolicy(sizePolicy)
        self.rightTextBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.rightTextBrowserButton)

        self.commandLineButton = QToolButton(self.centralwidget)
        self.commandLineButton.setObjectName(u"commandLineButton")
        sizePolicy.setHeightForWidth(self.commandLineButton.sizePolicy().hasHeightForWidth())
        self.commandLineButton.setSizePolicy(sizePolicy)
        self.commandLineButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.commandLineButton)

        self.showErrorBrowserButton = QToolButton(self.centralwidget)
        self.showErrorBrowserButton.setObjectName(u"showErrorBrowserButton")
        sizePolicy.setHeightForWidth(self.showErrorBrowserButton.sizePolicy().hasHeightForWidth())
        self.showErrorBrowserButton.setSizePolicy(sizePolicy)
        self.showErrorBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.showErrorBrowserButton)

        self.fileBrowserButton = QToolButton(self.centralwidget)
        self.fileBrowserButton.setObjectName(u"fileBrowserButton")
        sizePolicy.setHeightForWidth(self.fileBrowserButton.sizePolicy().hasHeightForWidth())
        self.fileBrowserButton.setSizePolicy(sizePolicy)
        self.fileBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.fileBrowserButton)

        self.historyBrowserButton = QToolButton(self.centralwidget)
        self.historyBrowserButton.setObjectName(u"historyBrowserButton")
        sizePolicy.setHeightForWidth(self.historyBrowserButton.sizePolicy().hasHeightForWidth())
        self.historyBrowserButton.setSizePolicy(sizePolicy)
        self.historyBrowserButton.setFont(font)

        self.buttomButtoNlAYOUT.addWidget(self.historyBrowserButton)

        self.buttomLayout.addLayout(self.buttomButtoNlAYOUT)

        self.textBrowser = QTextBrowser(self.centralwidget)
        self.textBrowser.setObjectName(u"textBrowser")

        self.buttomLayout.addWidget(self.textBrowser)

        self.buttomLayout.setStretch(0, 1)
        self.buttomLayout.setStretch(1, 8)

        self.gridLayout.addLayout(self.buttomLayout, 2, 1, 1, 1)

        self.leftButtonLayout = QVBoxLayout()
        self.leftButtonLayout.setSpacing(10)
        self.leftButtonLayout.setObjectName(u"leftButtonLayout")
        self.leftButtonLayout.setContentsMargins(5, 5, 5, 5)
        self.leftHintLabel = QLabel(self.centralwidget)
        self.leftHintLabel.setObjectName(u"leftHintLabel")
        font1 = QFont()
        font1.setFamily(u"Arial")
        font1.setPointSize(12)
        font1.setBold(True)
        font1.setWeight(75)
        self.leftHintLabel.setFont(font1)
        self.leftHintLabel.setFrameShape(QFrame.Box)
        self.leftHintLabel.setFrameShadow(QFrame.Raised)
        self.leftHintLabel.setLineWidth(2)
        self.leftHintLabel.setAlignment(Qt.AlignCenter)

        self.leftButtonLayout.addWidget(self.leftHintLabel)

        self.switchContainer = QWidget(self.centralwidget)
        self.switchContainer.setObjectName(u"switchContainer")
        sizePolicy1 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.switchContainer.sizePolicy().hasHeightForWidth())
        self.switchContainer.setSizePolicy(sizePolicy1)
        self.switchContainer.setMinimumSize(QSize(0, 30))
        self.switchContainer.setMaximumSize(QSize(16777215, 30))

        self.leftButtonLayout.addWidget(self.switchContainer)

        self.itemButton = QToolButton(self.centralwidget)
        self.itemButton.setObjectName(u"itemButton")
        sizePolicy.setHeightForWidth(self.itemButton.sizePolicy().hasHeightForWidth())
        self.itemButton.setSizePolicy(sizePolicy)
        font2 = QFont()
        font2.setPointSize(14)
        font2.setBold(True)
        font2.setWeight(75)
        self.itemButton.setFont(font2)

        self.leftButtonLayout.addWidget(self.itemButton)

        self.ocrButton = QToolButton(self.centralwidget)
        self.ocrButton.setObjectName(u"ocrButton")
        sizePolicy.setHeightForWidth(self.ocrButton.sizePolicy().hasHeightForWidth())
        self.ocrButton.setSizePolicy(sizePolicy)
        self.ocrButton.setFont(font2)

        self.leftButtonLayout.addWidget(self.ocrButton)

        self.toolButton = QToolButton(self.centralwidget)
        self.toolButton.setObjectName(u"toolButton")
        sizePolicy.setHeightForWidth(self.toolButton.sizePolicy().hasHeightForWidth())
        self.toolButton.setSizePolicy(sizePolicy)
        self.toolButton.setFont(font2)

        self.leftButtonLayout.addWidget(self.toolButton)

        self.newSketchButton = QToolButton(self.centralwidget)
        self.newSketchButton.setObjectName(u"newSketchButton")
        sizePolicy.setHeightForWidth(self.newSketchButton.sizePolicy().hasHeightForWidth())
        self.newSketchButton.setSizePolicy(sizePolicy)
        self.newSketchButton.setFont(font2)

        self.leftButtonLayout.addWidget(self.newSketchButton)

        self.clearSceneButton = QToolButton(self.centralwidget)
        self.clearSceneButton.setObjectName(u"clearSceneButton")
        sizePolicy.setHeightForWidth(self.clearSceneButton.sizePolicy().hasHeightForWidth())
        self.clearSceneButton.setSizePolicy(sizePolicy)
        self.clearSceneButton.setFont(font2)

        self.leftButtonLayout.addWidget(self.clearSceneButton)

        self.leftButtonLayout.setStretch(0, 1)
        self.leftButtonLayout.setStretch(1, 2)
        self.leftButtonLayout.setStretch(2, 3)
        self.leftButtonLayout.setStretch(3, 3)
        self.leftButtonLayout.setStretch(4, 3)
        self.leftButtonLayout.setStretch(5, 3)
        self.leftButtonLayout.setStretch(6, 3)

        self.gridLayout.addLayout(self.leftButtonLayout, 0, 0, 3, 1)

        self.topInputLayout = QHBoxLayout()
        self.topInputLayout.setSpacing(4)
        self.topInputLayout.setObjectName(u"topInputLayout")
        self.topInputLayout.setContentsMargins(2, 2, 2, 2)
        self.topHintLabel = QLabel(self.centralwidget)
        self.topHintLabel.setObjectName(u"topHintLabel")

        self.topInputLayout.addWidget(self.topHintLabel)

        self.inputLineEdit = QLineEdit(self.centralwidget)
        self.inputLineEdit.setObjectName(u"inputLineEdit")
        sizePolicy2 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.inputLineEdit.sizePolicy().hasHeightForWidth())
        self.inputLineEdit.setSizePolicy(sizePolicy2)

        self.topInputLayout.addWidget(self.inputLineEdit)

        self.topButtonLayout = QHBoxLayout()
        self.topButtonLayout.setSpacing(2)
        self.topButtonLayout.setObjectName(u"topButtonLayout")
        self.topButtonLayout.setContentsMargins(1, 1, 1, 1)
        self.searchButton = QToolButton(self.centralwidget)
        self.searchButton.setObjectName(u"searchButton")
        sizePolicy.setHeightForWidth(self.searchButton.sizePolicy().hasHeightForWidth())
        self.searchButton.setSizePolicy(sizePolicy)
        font3 = QFont()
        font3.setFamily(u"Arial")
        font3.setBold(True)
        font3.setWeight(75)
        self.searchButton.setFont(font3)

        self.topButtonLayout.addWidget(self.searchButton)

        self.analysisButton = QToolButton(self.centralwidget)
        self.analysisButton.setObjectName(u"analysisButton")
        sizePolicy.setHeightForWidth(self.analysisButton.sizePolicy().hasHeightForWidth())
        self.analysisButton.setSizePolicy(sizePolicy)
        self.analysisButton.setFont(font3)

        self.topButtonLayout.addWidget(self.analysisButton)

        self.clearInputButton = QToolButton(self.centralwidget)
        self.clearInputButton.setObjectName(u"clearInputButton")
        sizePolicy.setHeightForWidth(self.clearInputButton.sizePolicy().hasHeightForWidth())
        self.clearInputButton.setSizePolicy(sizePolicy)
        self.clearInputButton.setFont(font3)

        self.topButtonLayout.addWidget(self.clearInputButton)

        self.topInputLayout.addLayout(self.topButtonLayout)

        self.gridLayout.addLayout(self.topInputLayout, 0, 1, 1, 1)

        self.gridLayout.setRowStretch(0, 1)
        self.gridLayout.setRowStretch(1, 10)
        self.gridLayout.setRowStretch(2, 2)
        self.gridLayout.setColumnStretch(0, 1)
        self.gridLayout.setColumnStretch(1, 10)

        self.gridLayout_2.addLayout(self.gridLayout, 0, 0, 1, 1)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 960, 21))
        self.fileMenu = QMenu(self.menubar)
        self.fileMenu.setObjectName(u"fileMenu")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.fileMenu.menuAction())
        self.fileMenu.addAction(self.newDocAction)
        self.fileMenu.addAction(self.newProAction)
        self.fileMenu.addAction(self.openProAction)
        self.fileMenu.addAction(self.recentAction)
        self.fileMenu.addAction(self.saveAllAction)
        self.fileMenu.addAction(self.closeAllAction)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)

    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"cocr", None))
        self.newDocAction.setText(QCoreApplication.translate("MainWindow", u"\u65b0\u5efa\u6587\u6863", None))
        self.newProAction.setText(QCoreApplication.translate("MainWindow", u"\u65b0\u5efa\u5de5\u7a0b", None))
        self.openProAction.setText(QCoreApplication.translate("MainWindow", u"\u6253\u5f00\u9879\u76ee", None))
        self.recentAction.setText(QCoreApplication.translate("MainWindow", u"\u6700\u8fd1\u8bbf\u95ee", None))
        self.saveAllAction.setText(QCoreApplication.translate("MainWindow", u"\u4fdd\u5b58\u6240\u6709", None))
        self.closeAllAction.setText(QCoreApplication.translate("MainWindow", u"\u5173\u95ed\u6240\u6709", None))
        self.emptyBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u6e05\u7a7a", None))
        self.leftTextBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u5de6\u79fb", None))
        self.rightTextBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u53f3\u79fb", None))
        self.commandLineButton.setText(QCoreApplication.translate("MainWindow", u"\u547d\u4ee4\u884c", None))
        self.showErrorBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u9519\u8bef", None))
        self.fileBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u6587\u4ef6", None))
        self.historyBrowserButton.setText(QCoreApplication.translate("MainWindow", u"\u5386\u53f2\u8bb0\u5f55", None))
        self.leftHintLabel.setText(QCoreApplication.translate("MainWindow", u"\u6a21\u5f0f\u5207\u6362", None))
        self.itemButton.setText(QCoreApplication.translate("MainWindow", u"\u63a7\u4ef6", None))
        self.ocrButton.setText(QCoreApplication.translate("MainWindow", u"\u8bc6\u522b", None))
        self.toolButton.setText(QCoreApplication.translate("MainWindow", u"\u5de5\u5177", None))
        self.newSketchButton.setText(QCoreApplication.translate("MainWindow", u"\u65b0\u5efa", None))
        self.clearSceneButton.setText(QCoreApplication.translate("MainWindow", u"\u6e05\u7a7a", None))
        self.topHintLabel.setText(QCoreApplication.translate("MainWindow", u"\u8f93\u5165", None))
        self.searchButton.setText(QCoreApplication.translate("MainWindow", u"\u641c\u7d22", None))
        self.analysisButton.setText(QCoreApplication.translate("MainWindow", u"\u5206\u8bcd", None))
        self.clearInputButton.setText(QCoreApplication.translate("MainWindow", u"\u6e05\u7a7a", None))
        self.fileMenu.setTitle(QCoreApplication.translate("MainWindow", u"\u6587\u4ef6", None))
    # retranslateUi
