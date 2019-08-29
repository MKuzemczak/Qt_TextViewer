#include "gui.h"

GUI::GUI(QWidget *parent) : QWidget(parent)
{
    resize(1250,600);

    openButton_ = new QPushButton("Open", this);
    findButton_ = new QPushButton("Find", this);
    markButton_ = new QPushButton("Mark", this);
    reloadButton_ = new QPushButton("Reload", this);

    connect(openButton_, &QPushButton::clicked, this, &GUI::open);
    connect(findButton_, &QPushButton::clicked, this, &GUI::find);
    connect(markButton_, &QPushButton::clicked, this, &GUI::mark);
    connect(reloadButton_, &QPushButton::clicked, this, &GUI::reload);

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(openButton_);
    hbox->addWidget(findButton_);
    hbox->addWidget(markButton_);
    hbox->addWidget(reloadButton_);

    textEdit_ = new QPlainTextEdit(this);
    textEdit_->setReadOnly(true);
    textEdit_->setFont(QFont("Consolas", 10));

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addItem(hbox);
    vbox->addWidget(textEdit_);

    setLayout(vbox);

    setCurrentFile(QString());
    prevSearchedText_ = QString();
}

void GUI::openFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(filename), file.errorString()));
        return;
    }

    QTextStream in(&file);

    textEdit_->setPlainText(in.readAll());

    setCurrentFile(filename);

    if (filename.length() > 5 && filename.mid(filename.length() - 5, 5) == ".diff")
    {
        applyDiffStyle();
    }
    // statusBar()->showMessage(tr("File loaded"), 2000);
}

void GUI::markOccurencies(const QString &/*text*/)
{

}

void GUI::applyDiffStyle()
{

}

void GUI::setCurrentFile(const QString &filename)
{
    currentFile_ = filename;
}

void GUI::setPrevSearchedText(const QString &text)
{
    prevSearchedText_ = text;
}

QString GUI::getPrevSearchedText()
{
    return prevSearchedText_;
}

void GUI::open()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if (!filename.isEmpty())
    {
        openFile(filename);
    }
}

void GUI::reload()
{
    openFile(currentFile_);
}

void GUI::find()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Find", "Enter text:", QLineEdit::Normal, getPrevSearchedText(), &ok);
    textEdit_->find(text);
    setPrevSearchedText(text);
}

void GUI::mark()
{

}
