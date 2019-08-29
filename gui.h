#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

class GUI : public QWidget
{
    Q_OBJECT

    QPushButton *openButton_,
        *findButton_,
        *markButton_,
        *reloadButton_;
    QPlainTextEdit *textEdit_;
    QString currentFile_,
        prevSearchedText_;

public:
    explicit GUI(QWidget *parent = nullptr);

    void openFile(const QString & filename);
    void markOccurencies(const QString & text);
    void applyDiffStyle();

    void setCurrentFile(const QString & filename);
    void setPrevSearchedText(const QString & text);
    QString getPrevSearchedText();

signals:

public slots:
    void open();
    void reload();
    void find();
    void mark();
};

#endif // GUI_H
