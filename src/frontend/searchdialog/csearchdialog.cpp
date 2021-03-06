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

#include "csearchdialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QRegExp>
#include <QVBoxLayout>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/cswordmodulesearch.h"
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletimeapp.h"
#include "../btmoduleindexdialog.h"
#include "../messagedialog.h"
#include "btindexdialog.h"
#include "btsearchoptionsarea.h"
#include "btsearchresultarea.h"


namespace {
const QString GeometryKey = "GUI/SearchDialog/geometry";
} // anonymous namespace

namespace Search {

static CSearchDialog* m_staticDialog = nullptr;

void CSearchDialog::openDialog(const BtConstModuleList modules,
                               const QString &searchText, QWidget *parentDialog)
{
    if (!m_staticDialog)
        m_staticDialog = new CSearchDialog(parentDialog);
    m_staticDialog->reset();

    if (modules.count()) {
        m_staticDialog->m_searchOptionsArea->setModules(modules);
    }
    else {
        m_staticDialog->showModulesSelector();
    }

    m_staticDialog->setSearchText(searchText);
    if (m_staticDialog->isHidden()) {
        m_staticDialog->show();
    }

    if (modules.count() && !searchText.isEmpty()) {
        m_staticDialog->startSearch();
    }
    // moved these to after the startSearch() because
    // the progress dialog caused them to loose focus.
    m_staticDialog->raise();
    m_staticDialog->activateWindow();
}

void CSearchDialog::closeDialog() {
    if (m_staticDialog != nullptr)
        m_staticDialog->closeButtonClicked();
}

CSearchDialog* CSearchDialog::getSearchDialog() {
    BT_ASSERT(m_staticDialog);
    return m_staticDialog;
}

CSearchDialog::CSearchDialog(QWidget *parent)
        : QDialog(parent), /*m_searchButton(0),*/ m_closeButton(nullptr),
        m_searchResultArea(nullptr), m_searchOptionsArea(nullptr) {
    setWindowIcon(CResMgr::searchdialog::icon());
    setWindowTitle(tr("Search"));
    setAttribute(Qt::WA_DeleteOnClose);

    initView();
    initConnections();
}

CSearchDialog::~CSearchDialog() {
    saveDialogSettings();
    m_staticDialog = nullptr;
}

void CSearchDialog::startSearch() {
    QString originalSearchText(m_searchOptionsArea->searchText());

    // first check the search string for errors
    {
        QString TestString(originalSearchText);
        QRegExp ReservedWords("heading:|footnote:|morph:|strong:");
        if (TestString.replace(ReservedWords, "").simplified().isEmpty()) {
            return;
        }
    }
    QString searchText = CSwordModuleSearch::prepareSearchText(originalSearchText, m_searchOptionsArea->searchType());

    // Insert search text into history list of combobox
    m_searchOptionsArea->addToHistory(originalSearchText);

    auto searchModules = modules();

    // Check that we have the indices we need for searching
    /// \warning indexing is some kind of internal optimization, so we leave
    /// modules const, but unconst them here only
    QList<CSwordModuleInfo*> unindexedModules;
    for (auto const * const m : searchModules)
        if (!m->hasIndex())
            unindexedModules.append(const_cast<CSwordModuleInfo*>(m));

    if (unindexedModules.size() > 0) {
        // Build the list of module names:
        QStringList moduleNameList;
        for (auto const * const m : unindexedModules)
            moduleNameList.append(m->name());
        QString moduleNames("<br><center>");
        moduleNames.append(moduleNameList.join(", "));
        moduleNames.append("</center><br>");

        // Ask the user about unindexed modules:
        int result = message::showQuestion(
                this, tr("Missing indices"),
                tr("The following modules need to be indexed before they can be"
                   " searched in:") + moduleNames + tr("Indexing could take a l"
                   "ong time. Click \"Yes\" to index the modules and start the "
                   "search, or \"No\" to cancel the search."),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        // User didn't press "Yes":
        if ((result & (QMessageBox::Yes | QMessageBox::Default)) == 0x0) {
            return;
        }

        // Show indexing dialog, and index the modules:
        if (!BtModuleIndexDialog::indexAllModules(unindexedModules)) {
            // Failed or user cancelled.
            return;
        }
    }

    // Disable the dialog:
    setEnabled(false);
    setCursor(Qt::WaitCursor);

    // Execute search:
    CSwordModuleSearch::Results searchResult;
    try {
        searchResult =
                CSwordModuleSearch::search(searchText,
                                           searchModules,
                                           m_searchOptionsArea->searchScope());
    } catch (...) {
        QString msg;
        try {
            throw;
        } catch (std::exception const & e) {
            msg = e.what();
        } catch (...) {
            msg = tr("<UNKNOWN EXCEPTION>");
        }

        message::showWarning(this,
                             tr("Search aborted"),
                             tr("An internal error occurred while executing "
                                "your search:<br/><br/>%1").arg(msg));
        // Re-enable the dialog:
        setEnabled(true);
        setCursor(Qt::ArrowCursor);
        return;
    }

    // Display the search results:
    if (!searchResult.empty()) {
        m_searchResultArea->setSearchResult(std::move(searchResult));
    } else {
        m_searchResultArea->reset();
    }
    m_staticDialog->raise();
    m_staticDialog->activateWindow();

    // Re-enable the dialog:
    setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

/** Sets the search text which is used for the search. */
void CSearchDialog::setSearchText( const QString &searchText ) {
    m_searchOptionsArea->setSearchText(searchText);
}

/** Initializes this object. */
void CSearchDialog::initView() {

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    setLayout(verticalLayout);

    m_searchOptionsArea = new BtSearchOptionsArea(this);
    verticalLayout->addWidget(m_searchOptionsArea);

    m_searchResultArea = new BtSearchResultArea(this);
    m_searchResultArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalLayout->addWidget(m_searchResultArea);

    QLabel* hint = new QLabel(tr("Drag any verse reference onto an open Bible window"), this);
    verticalLayout->addWidget(hint);

    QHBoxLayout* horizontalLayout = new QHBoxLayout();

    QSpacerItem* spacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(spacerItem);

    m_analyseButton = new QPushButton(tr("&Analyze results..."), nullptr);
    m_analyseButton->setToolTip(tr("Show a graphical analysis of the search result"));
    horizontalLayout->addWidget(m_analyseButton);

    m_manageIndexes = new QPushButton(tr("&Manage Indexes..."), nullptr);
    m_manageIndexes->setToolTip(tr("Recreate search indexes"));
    horizontalLayout->addWidget(m_manageIndexes);

    m_closeButton = new QPushButton(this);
    m_closeButton->setText(tr("&Close"));
    m_closeButton->setIcon(CResMgr::searchdialog::icon_close());
    horizontalLayout->addWidget(m_closeButton);

    verticalLayout->addLayout(horizontalLayout);

    loadDialogSettings();
}

void CSearchDialog::showModulesSelector() {
    m_searchOptionsArea->chooseModules();
}

/** Initializes the signal slot connections */
void CSearchDialog::initConnections() {
    // Search button is clicked
    BT_CONNECT(m_searchOptionsArea->searchButton(), &QPushButton::clicked,
               [this] { startSearch(); });
    // Return/Enter is pressed in the search text field
    BT_CONNECT(m_searchOptionsArea, &BtSearchOptionsArea::sigStartSearch,
               [this] { startSearch(); });
    BT_CONNECT(m_closeButton, &QPushButton::clicked,
               this, &CSearchDialog::closeButtonClicked);

    BT_CONNECT(m_analyseButton, &QPushButton::clicked,
               m_searchResultArea, &BtSearchResultArea::showAnalysis);

    BT_CONNECT(m_manageIndexes, &QPushButton::clicked,
               [this] { BtIndexDialog(this).exec(); });
}

/** Resets the parts to the default. */
void CSearchDialog::reset() {
    m_searchOptionsArea->reset();
    m_searchResultArea->reset();
}

void CSearchDialog::closeButtonClicked() {
    // With Qt::WA_DeleteOnClose set, the dialog will be deleted now
    m_staticDialog->close();
}

void CSearchDialog::loadDialogSettings() {
    restoreGeometry(btConfig().value<QByteArray>(GeometryKey, QByteArray()));
}

void CSearchDialog::saveDialogSettings() const {
    btConfig().setValue(GeometryKey, saveGeometry());
}


} //end of namespace Search
