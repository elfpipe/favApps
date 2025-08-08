#include <QApplication>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>

class CppHighlighter : public QSyntaxHighlighter {
public:
    CppHighlighter(QTextDocument *parent = nullptr)
        : QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);

        const QStringList keywords = {
            "int", "float", "double", "char", "void",
            "class", "struct", "enum", "public", "private", "protected",
            "if", "else", "while", "for", "return", "switch", "case",
            "break", "continue", "namespace", "using", "new", "delete",
            "const", "static", "virtual", "override", "this", "nullptr"
        };

        for (const QString &word : keywords) {
            HighlightingRule rule;
            rule.pattern = QRegularExpression("\\b" + word + "\\b");
            rule.format = keywordFormat;
            rules.append(rule);
        }

        QTextCharFormat stringFormat;
        stringFormat.setForeground(Qt::darkGreen);
        rules.append({ QRegularExpression("\".*?\""), stringFormat });

        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::darkGray);
        rules.append({ QRegularExpression("//[^\n]*"), commentFormat });
    }

protected:
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : rules) {
            auto it = rule.pattern.globalMatch(text);
            while (it.hasNext()) {
                auto match = it.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;
};

class CodeEditor : public QPlainTextEdit {
public:
    CodeEditor(QWidget *parent = nullptr) : QPlainTextEdit(parent) {
        // setPlainText("// Your C++ code here\n");
        setFont(QFont("Courier", 10)); // safer default font
        new CppHighlighter(document());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CodeEditor editor;
    QString fileName;
    if(argc == 2) {
        fileName = argv[1];
    } else {
        fileName = QFileDialog::getOpenFileName(&editor, "Open File");
    }
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            editor.setPlainText(in.readAll());
        } else {
            QMessageBox::warning(&editor, "Error", "Could not open file.");
        }
    }
    editor.setWindowTitle("CodeEditor");
    editor.resize(800, 600);
    editor.show();
    return app.exec();
}
