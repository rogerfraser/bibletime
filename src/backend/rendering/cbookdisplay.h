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

#include "centrydisplay.h"

#include "ctextrendering.h"


class CSwordTreeKey;

namespace Rendering {

/**
* \brief CEntryDisplay implementation for GenBook modules,

  A CEntryDisplay implementation which works on tree-based GenBook modules of
  Sword.
*/
class CBookDisplay: public CEntryDisplay {

    public: /* Methods: */

        const QString text(const BtConstModuleList &modules,
                           const QString &key,
                           const DisplayOptions &displayOptions,
                           const FilterOptions &filterOptions) override;

}; /* class CBookDisplay */

} /* namespace Rendering */
