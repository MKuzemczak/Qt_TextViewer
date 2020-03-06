#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTextBlock>
#include <QTextDocumentFragment>

#include "dtl/dtl.hpp"
#include "codeedit.h"

class GUI : public QMainWindow
{
    Q_OBJECT

    QPushButton *openButton_,
        *findButton_,
        *markButton_,
        *reloadButton_;
    CodeEdit *textEdit_;
    QString currentFile_,
        prevSearchedText_,
        prevMarkedText_;
    bool randomColorMark_;

    void createActions();
public:
    explicit GUI(QWidget *parent = nullptr);

    void openFile(const QString & filename);
    void markOccurencies(const QString & text, const QColor &color);
    void applyDiffStyle();
    QString applyJpegStyle(const char * inByteArray, const long long size);

    void setCurrentFile(const QString & filename);
    void setPrevSearchedText(const QString & text);
    QString getPrevSearchedText();
    void setPrevMarkedText(const QString & text);
    QString getPrevMarkedText();

signals:

public slots:
    void open();
    void reload();
    void find();
    void mark();
    void setRandomColorMark(bool);
};

#endif // GUI_H
