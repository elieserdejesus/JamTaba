#include "SonarTextEditorModifier.h"

#include "KeyboardHook.h"

SonarTextEditorModifier::SonarTextEditorModifier()
{
    keyboardHook::installLowLevelKeyboardHook();
}

SonarTextEditorModifier::~SonarTextEditorModifier()
{
    keyboardHook::uninstallLowLevelKeyboardKook();
}

void SonarTextEditorModifier::modify(QLineEdit *lineEdit, bool finishOnReturnKeyPressed)
{
    Q_UNUSED(lineEdit);
    Q_UNUSED(finishOnReturnKeyPressed);
}

void SonarTextEditorModifier::modify(QComboBox *comboBox)
{
    Q_UNUSED(comboBox);
}
