#ifndef TEXT_EDITOR_FACTORY
#define TEXT_EDITOR_FACTORY

#include <QLineEdit>

class TextEditorFactory
{
public:
    virtual QLineEdit *createTextEditor(QWidget *parent, const QString &initialText = "") = 0;
};

#endif
