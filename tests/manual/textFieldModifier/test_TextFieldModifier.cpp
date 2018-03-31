#include <QApplication>
#include <QLineEdit>
#include <QMainWindow>
#include <QBoxLayout>
#include <QAction>
#include <QDebug>
#include <QTextEdit>

#include "IconFactory.h"
#include "TopLevelTextEditorModifier.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    auto lineEdit = new QLineEdit("testing");

    auto centralWidget = new QWidget();
    auto layout = new QVBoxLayout();
    centralWidget->setLayout(layout);
    mainWindow.setCentralWidget(centralWidget);


    auto textWidget = new QTextEdit("testing top level editor");
    textWidget->setReadOnly(true);
    textWidget->setVisible(false);

    layout->addWidget(lineEdit);
    layout->addWidget(textWidget);

    TopLevelTextEditorModifier modifier;

    auto emojiIcon = IconFactory::createChatEmojiIcon(Qt::red);
    auto action = lineEdit->addAction(emojiIcon, QLineEdit::LeadingPosition);
    QObject::connect(action, &QAction::triggered, [&](){
        textWidget->setVisible(!textWidget->isVisible());
        mainWindow.resize(textWidget->isVisible() ? 400 : 300, mainWindow.height());
    });

    modifier.modify(lineEdit, true);

    mainWindow.show();

    return app.exec();
}
