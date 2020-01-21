#include "gui.h"

#include <fstream>


GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
    resize(1920,600);

    textEdit_ = new CodeEdit(this);
    textEdit_->setReadOnly(true);
    textEdit_->setTextInteractionFlags(textEdit_->textInteractionFlags() | Qt::TextSelectableByKeyboard);
    textEdit_->setFont(QFont("Consolas", 10));

    setCentralWidget(textEdit_);
    createActions();

    setCurrentFile(QString());
    prevSearchedText_ = QString();
    randomColorMark_ = false;
}

void GUI::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu("File");
    QAction *openAct = new QAction(/*openIcon,*/ "Open...", this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &GUI::open);
    fileMenu->addAction("Open", this, &GUI::open, QKeySequence::Open);
    fileMenu->addAction("Reload", this, &GUI::reload, QKeySequence::Refresh);

    QMenu *editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction("Find", this, &GUI::find, QKeySequence::Find);
    editMenu->addAction("Mark", this, &GUI::mark, QKeySequence(tr("Ctrl+M", "Edit|Mark")));

    QAction *markRandomAct = new QAction("Mark with random color", this);
    markRandomAct->setCheckable(true);
    connect(markRandomAct, &QAction::toggled, this, &GUI::setRandomColorMark);
    editMenu->addAction(markRandomAct);
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
}

void GUI::markOccurencies(const QString &text, const QColor &color)
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(textEdit_->extraSelections());
    QTextCursor cursor = textEdit_->textCursor();
    while ( textEdit_->find(text) )
    {
        QTextEdit::ExtraSelection extra;
        extra.cursor = textEdit_->textCursor();
        extra.format.setBackground(QBrush(color));
        extraSelections.append(extra);
    }

    textEdit_->setExtraSelections(extraSelections);
    textEdit_->setTextCursor(cursor);

    setPrevMarkedText(text);
}

void GUI::applyDiffStyle()
{
    typedef std::vector<std::pair<char, char>> ChangeVector;

    struct Change
    {
        int pos, stretch;
        QColor color;
        int blockPos;
        bool marked;
    };

    auto markLambda = [](QList<QTextEdit::ExtraSelection> & extras, const Change & change, QTextCursor & cursor)
    {
        int blockNumDiff = cursor.blockNumber() - change.blockPos;

        QTextCursor::MoveOperation operation;

        if (blockNumDiff < 0)
        {
            operation = QTextCursor::MoveOperation::Down;
        }
        else
        {
            operation = QTextCursor::MoveOperation::Up;
        }

        QTextEdit::ExtraSelection extra;
        cursor.movePosition(operation, QTextCursor::MoveMode::MoveAnchor, abs(blockNumDiff));
        cursor.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::MoveMode::MoveAnchor);
        cursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, change.pos);
        cursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, change.stretch);
        extra.cursor = cursor;
        extra.format.setBackground(QBrush(change.color));
        extras.append(extra);
    };

    QColor yellow(255, 255, 102),
            lightRed(255, 204, 204),
            lightGreen(204, 255, 204),
            red(255, 100, 100),
            green(0, 230, 0);
    QString text = textEdit_->toPlainText(),
            addLines = "", delLines = "";
    std::vector<Change> changes;
    int addBlockPos = 0, delBlockPos = 0, addCntr = 0, delCntr = 0;
    bool newChangedFragment = false;
    char prevLineStart = ' ';
    bool startOfBlock = false;

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(textEdit_->extraSelections());
    QTextCursor cursor = textEdit_->textCursor();
    cursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor);

    while (cursor.movePosition(QTextCursor::MoveOperation::Down, QTextCursor::MoveMode::MoveAnchor))
    {
        cursor.select(QTextCursor::SelectionType::LineUnderCursor);
        QString line = cursor.selection().toPlainText();
        if (line.mid(0, 3) == "+++" || line.mid(0, 3) == "---")
        {
            prevLineStart = ' ';
            QTextEdit::ExtraSelection extra;
            extra.cursor = cursor;
            extra.format.setBackground(QBrush(yellow));
            extraSelections.append(extra);
        }
        else if (line[0] == '+' || line[0] == '-')
        {
            newChangedFragment = true;

            do
            {
                cursor.select(QTextCursor::SelectionType::LineUnderCursor);
                line = cursor.selection().toPlainText();
                QTextEdit::ExtraSelection extra;
                extra.cursor = cursor;

                if(prevLineStart != line[0])
                    startOfBlock = true;

                if (line[0] == '+')
                {
                    prevLineStart = '+';
                    addLines.append(" " + line.mid(1, line.length() - 1) + '\n');
                    extra.format.setBackground(QBrush(lightGreen));
                    if (startOfBlock)
                    {
                        addBlockPos = cursor.blockNumber();
                        startOfBlock = false;
                    }
                }
                else if (line[0] == '-')
                {
                    prevLineStart = '-';
                    delLines.append(" " + line.mid(1, line.length() - 1) + '\n');
                    extra.format.setBackground(QBrush(lightRed));
                    if (startOfBlock)
                    {
                        delBlockPos = cursor.blockNumber();
                        startOfBlock = false;
                    }
                }
                else {
                    prevLineStart = ' ';
                    break;
                }
                extraSelections.append(extra);
                cursor.clearSelection();
            } while (cursor.movePosition(QTextCursor::MoveOperation::Down, QTextCursor::MoveMode::MoveAnchor));
        }
        else {
            prevLineStart = ' ';
        }

        cursor.select(QTextCursor::SelectionType::LineUnderCursor);
        line = cursor.selection().toPlainText();

        if (newChangedFragment == true)
        {
            prevLineStart = ' ';
            newChangedFragment = false;
            if (((double)addLines.length() / (double)delLines.length()) > 0.1 &&
                    ((double)addLines.length() / (double)delLines.length()) < 10)
            {
                dtl::Diff<char, std::string> dif(delLines.toStdString(), addLines.toStdString());
                dif.compose();
                ChangeVector changeVector = dif.getChangeVector();
                QTextCursor tmpCursor = cursor;

                changes.push_back(Change{0, 0, QColor(), 0, false});

                for (size_t i = 0; i < changeVector.size(); i++)
                {
                    switch(changeVector[i].first)
                    {
                    case ' ':
                        if (changes[changes.size() - 1].stretch > 0)
                        {
                            markLambda(extraSelections, changes[changes.size() - 1], cursor);
                            changes[changes.size() - 1].marked = true;
                            changes.push_back(Change{0, 0, QColor(), 0, false});
                        }
                        ++addCntr;
                        ++delCntr;
                        break;
                    case '+':
                        if (changes[changes.size() - 1].stretch > 0 &&
                                changes[changes.size() - 1].color != green)
                        {
                            markLambda(extraSelections, changes[changes.size() - 1], cursor);
                            changes[changes.size() - 1].marked = true;
                            changes.push_back(Change{0, 0, QColor(), 0, false});
                        }
                        if (changes[changes.size() - 1].stretch == 0)
                        {
                            changes[changes.size() - 1].pos = addCntr;
                            changes[changes.size() - 1].color = green;
                            changes[changes.size() - 1].blockPos = addBlockPos;
                        }
                        ++addCntr;
                        ++(changes[changes.size() - 1].stretch);
                        break;
                    case '-':
                        if (changes[changes.size() - 1].stretch > 0 &&
                                changes[changes.size() - 1].color != red)
                        {
                            markLambda(extraSelections, changes[changes.size() - 1], cursor);
                            changes[changes.size() - 1].marked = true;
                            changes.push_back(Change{0, 0, QColor(), 0, false});
                        }
                        if (changes[changes.size() - 1].stretch == 0)
                        {
                            changes[changes.size() - 1].pos = delCntr;
                            changes[changes.size() - 1].color = red;
                            changes[changes.size() - 1].blockPos = delBlockPos;
                        }
                        ++delCntr;
                        ++(changes[changes.size() - 1].stretch);
                        break;
                    }
                }
                if (changes[changes.size() - 1].stretch == 0)
                    changes.pop_back();
                if (!(changes[changes.size() - 1].marked))
                    markLambda(extraSelections, changes[changes.size() - 1], cursor);
                changes.clear();

                cursor = tmpCursor;
            }

            addCntr = 0;
            delCntr = 0;
            addLines = "";
            delLines = "";
        }
    }
    textEdit_->setExtraSelections(extraSelections);
    cursor.clearSelection();
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

void GUI::setPrevMarkedText(const QString &text)
{
    prevMarkedText_ = text;
}

QString GUI::getPrevMarkedText()
{
    return prevMarkedText_;
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
    if (ok && !text.isEmpty())
    {
        textEdit_->find(text);
        setPrevSearchedText(text);
    }
}

void GUI::mark()
{
    QString text = textEdit_->textCursor().selection().toPlainText();
    QColor color;
    if (randomColorMark_)
        color = QColor::fromRgb(QRandomGenerator::global()->generate());
    else
        color = QColorDialog::getColor(Qt::green, this, "Select Color");

    if (!text.isEmpty() && color.isValid())
    {
        markOccurencies(text, color);
    }
}

void GUI::setRandomColorMark(bool var)
{
    randomColorMark_ = var;
}
