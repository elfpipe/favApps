/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include <QTextDocument>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QShortcut>
#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

#include <QMessageBox>

#include <QBoxLayout>

#include "codeeditor.h"

//![constructor]

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
	init();
}

void CodeEditor::init()
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    window = new QWidget;

	QHBoxLayout *buttonLayout = new QHBoxLayout;

	QPushButton *loadButton = new QPushButton("Load..");
	connect(loadButton, SIGNAL(released()), this, SLOT(loadFile()));
	QPushButton *saveAsButton = new QPushButton("Save as..");
	connect(saveAsButton, SIGNAL(released()), this, SLOT(saveAsFile()));

	buttonLayout->addWidget(loadButton);
	buttonLayout->addWidget(saveAsButton);

	// QShortcut *shortcut = new QShortcut(QKeySequence(tr("Ctrl+S")), window);
	// connect(shortcut, SIGNAL(activated()), this, SLOT(saveFile()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(buttonLayout);
	layout->addWidget(this);
	setParent(window);

    setFont(QFont("DejaVu Sans Mono", 10)); // safer default font
    setPlainText("// Quick and dirty C++ editor\n"
                    "/* Multi-line comment test\n"
                    "   Still in comment */\n"
                    "int main() {\n"
                    "    return 0; // end\n"
                    "}");

	new Highlighter(document());

	window->setLayout(layout);
	window->show();

	// QFont font;
	// font.setFamily("DevaVu Sans Mono");
	// font.setStyleHint(QFont::Monospace);
	// font.setFixedPitch(true);
	// font.setPointSize(10);

	// setFont(font);

	// const int tabStop = 4;

	// QFontMetrics metrics(font);
	// setTabStopDistance(tabStop * metrics.horizontalAdvance(' '));
}

CodeEditor::CodeEditor(char *filename, QWidget *parent)
{
	init();

	this->filename = QString(filename);
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::information(0, "error", file.errorString());
		return;
	}
	setPlainText(file.readAll());
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
        saveFile();
        return; // Skip normal processing
    }
    QPlainTextEdit::keyPressEvent(event); // Default behavior
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    //keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
//! [0] //! [1]
    }
//! [1]

//! [2]
    QTextCharFormat classFormat;
    classFormat.setForeground(Qt::darkMagenta);
    highlightingRules.append({QRegularExpression("\\bQ[A-Za-z]+\\b"), classFormat});
//! [2]

//! [3]
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkRed);
    highlightingRules.append({QRegularExpression("//[^\n]*"), singleLineCommentFormat});

//! [3]

//! [4]
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);
    highlightingRules.append({QRegularExpression("\".*\""), quotationFormat});
//! [4]

//! [5]

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    highlightingRules.append({QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat});
//! [5]

//! [6]
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
    multiLineCommentFormat.setForeground(Qt::darkRed);

    // QTextCharFormat commentFormat;
    // commentFormat.setForeground(Qt::darkGray);
    // rules.append({ QRegularExpression("//[^\n]*"), commentFormat });
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    // Apply single-line rules
        for (const auto &rule : highlightingRules) {
            auto it = rule.pattern.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        // Multi-line comments
        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(commentStartExpression);

        while (startIndex >= 0) {
            auto match = commentEndExpression.match(text, startIndex);
            int endIndex = match.capturedStart();
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
        }}
//! [11]



void CodeEditor::loadFile()
{
	if (document()->isModified()) {
		QMessageBox::information(0, "Content changed. Exit or save before reload.", "Warning.");
		return;
	}
	document()->setModified(false);

	filename = QFileDialog::getOpenFileName(this);
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::information(this, "error", file.errorString());
		return;
	}

	setPlainText(file.readAll());
	window->setWindowTitle(filename);
}

void CodeEditor::saveAsFile()
{
	filename = QFileDialog::getSaveFileName(this);
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::information(0, "error", file.errorString());
		return;
	}
	QTextStream stream(&file);
	stream << toPlainText();

	document()->setModified(false);
}

void CodeEditor::saveFile()
{
    printf("save %s\n", filename.toLocal8Bit().constData());
	if(filename.size() == 0) {
		saveAsFile();
		return;
	}
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::information(0, "error", file.errorString());
		return;
	}
	QTextStream stream(&file);
	stream << toPlainText();

	document()->setModified(false);
}

//![constructor]

//![extraAreaWidth]

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

//![extraAreaWidth]

//![slotUpdateExtraAreaWidth]

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{ 
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//![slotUpdateExtraAreaWidth]

//![slotUpdateRequest]

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

//![slotUpdateRequest]

//![resizeEvent]

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//![resizeEvent]

void CodeEditor::closeEvent(QCloseEvent *event)
{
	if(document()->isModified()) {
		QMessageBox::StandardButton reply = QMessageBox::question(this, "Contents modified", "Save before exit?", QMessageBox::Yes|QMessageBox::No);
		if(reply == QMessageBox::Yes) {
			saveFile();
		}
	}
	event->accept();
}

void CodeEditor::insertFromMimeData(const QMimeData *source)
{
    QTextCursor cursor = textCursor();
    cursor.insertText(source->text());
}
    
//![cursorPositionChanged]

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


//![cursorPositionChanged]

//![extraAreaPaintEvent_0]

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
//![extraAreaPaintEvent_2]

