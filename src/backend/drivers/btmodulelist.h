/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTMODULELIST_H
#define BTMODULELIST_H

#include <QList>

class CSwordModuleInfo;

typedef QList<CSwordModuleInfo *> BtModuleList;/*
typedef QList<CSwordModuleInfo const *> BtConstModuleList;*/

class BtConstModuleList : public QList<CSwordModuleInfo const *> {
public: /* Methods */
    inline BtConstModuleList() {}
    inline BtConstModuleList(BtModuleList const & other) {
        foreach(CSwordModuleInfo *m, other)
            this->append(m);
    }
    inline BtConstModuleList(QList<CSwordModuleInfo const *> const & other) {
        foreach(CSwordModuleInfo const *m, other)
            this->append(m);
    }
};

#endif // BTMODULELIST_H