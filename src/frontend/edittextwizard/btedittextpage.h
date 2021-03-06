/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QWizardPage>
#include <QTextCharFormat>

class BtEditTextWizard;
class QTextEdit;
class QToolBar;
class QVBoxLayout;

class BtEditTextPage final: public QWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtEditTextPage(QWidget * parent = nullptr);

    void setFont(const QFont& font);
    void setText(const QString& text);
    void setTitle(const QString& text);
    QString text() const;

    void cleanupPage() final override;
    void initializePage() final override;
    bool validatePage() final override;

Q_SIGNALS:

    void signalFontChanged(const QFont &);
    void signalFontColorChanged(const QColor &);
    void signalFontSizeChanged(int);

private:
    void alignmentChanged(int);
    void initActions();
    QFont initHtmlFont();
    void retranslateUi();
    void setupToolBar();


    QTextEdit * m_plainTextEdit;
    QTextEdit * m_htmlTextEdit;
    QToolBar * m_toolBar;
    BtEditTextWizard * m_wizard;
    bool m_handingFormatChangeFromEditor;

    bool m_htmlMode;
    QFont m_plainTextFont;
    QString m_text;

    struct {
        QAction* bold;
        QAction* italic;
        QAction* underline;

        QAction* alignLeft;
        QAction* alignCenter;
        QAction* alignRight;
    }
    m_actions;
};
