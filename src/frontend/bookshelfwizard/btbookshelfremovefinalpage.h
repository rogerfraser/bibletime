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

#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/drivers/btmoduleset.h"


class BtBookshelfWizard;
class QLabel;
class QVBoxLayout;

class BtBookshelfRemoveFinalPage final: public BtBookshelfWizardPage {

    Q_OBJECT

public: /* Methods: */

    BtBookshelfRemoveFinalPage(QWidget * parent = nullptr);

    bool isComplete() const final override;
    void initializePage() final override;
    int nextId() const final override;

private: /* Methods: */

    void retranslateUi();

private: /* Fields: */

    QLabel * m_msgLabel;
    QVBoxLayout * m_verticalLayout;

}; /* class BtBookshelfRemoveFinalPage */
