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

#include "btbookshelfwizardpage.h"

#include "btbookshelfwizardenums.h"


class QGroupBox;
class QLabel;
class QRadioButton;
class QVBoxLayout;

class BtBookshelfTaskPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfTaskPage(QWidget * parent = nullptr);

    int nextId() const final override;
    WizardTaskType taskType() const;

private: /* Methods: */

    bool timeToUpdate() const;
    void retranslateUi();

private: /* Fields: */

    QLabel * m_warningLabel;
    QGroupBox * m_groupBox;
    QVBoxLayout * m_verticalLayout;
    QRadioButton * m_installRadioButton;
    QRadioButton * m_updateRadioButton;
    QRadioButton * m_removeRadioButton;

};
