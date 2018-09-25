// Copyright (c) 2018 The Blocknet Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "blocknetaddressbook.h"
#include "blockneticonaltbtn.h"
#include "blocknetlabelbtn.h"
#include "blocknetavatar.h"

#include <QHeaderView>
#include <QApplication>

BlocknetAddressBook::BlocknetAddressBook(QWidget *popup, QFrame *parent) : QFrame(parent), popupWidget(popup), layout(new QVBoxLayout) {
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->setContentsMargins(46, 10, 50, 0);
    this->setLayout(layout);

    titleLbl = new QLabel(tr("Address Book"));
    titleLbl->setObjectName("h4");
    titleLbl->setFixedHeight(26);

    auto *topBox = new QFrame;
    auto *topBoxLayout = new QHBoxLayout;
    topBoxLayout->setContentsMargins(QMargins());
    topBox->setLayout(topBoxLayout);

    auto *addAddressBtn = new BlocknetIconAltBtn(":/redesign/QuickActions/AddressButtonIcon.png", 5);

    addButtonLbl = new QLabel(tr("Add Address"));
    addButtonLbl->setObjectName("h4");

    filterLbl = new QLabel(tr("Filter by:"));
    filterLbl->setObjectName("title");

    addressDropdown = new BlocknetDropdown;
    addressDropdown->addItem(tr("All Addresses"), FILTER_ALL);
    addressDropdown->addItem(tr("Sending"),      FILTER_SENDING);
    addressDropdown->addItem(tr("Receiving"),  FILTER_RECEIVING);

    table = new QTableWidget;
    table->setContentsMargins(QMargins());
    table->setColumnCount(COLUMN_DELETE + 1);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->setAlternatingRowColors(true);
    table->setColumnWidth(COLUMN_ACTION, 50);
    table->setColumnWidth(COLUMN_AVATAR, 50);
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setDefaultSectionSize(78);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table->horizontalHeader()->setSortIndicatorShown(true);
    table->horizontalHeader()->setSectionsClickable(true);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_ACTION, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_AVATAR, QHeaderView::Fixed);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_ALIAS, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_ADDRESS, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_COPY, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_EDIT, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(COLUMN_DELETE, QHeaderView::Stretch);
    table->setHorizontalHeaderLabels({ "", "", tr("Alias"), tr("Address"), "", "", "" });

    addressTbl = new BlocknetAddressBookTable(popup);
    //addressTbl->setObjectName("addressBookTable");
    addressTbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addressTbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addressTbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    addressTbl->setSelectionMode(QAbstractItemView::ExtendedSelection);
    addressTbl->setAlternatingRowColors(true);
    addressTbl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    addressTbl->setShowGrid(false);
    addressTbl->setContextMenuPolicy(Qt::CustomContextMenu);
    addressTbl->setSortingEnabled(true);
    addressTbl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    addressTbl->verticalHeader()->setDefaultSectionSize(78);
    addressTbl->verticalHeader()->setVisible(false);
    addressTbl->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    addressTbl->horizontalHeader()->setSortIndicatorShown(true);
    addressTbl->horizontalHeader()->setSectionsClickable(true);

    // Set transaction data and adjust section sizes
    //addressTbl->setWalletModel(walletModel);
    addressTbl->setColumnWidth(COLUMN_ACTION, 50);
    addressTbl->setColumnWidth(COLUMN_AVATAR, 50);

    topBoxLayout->addWidget(addAddressBtn, Qt::AlignLeft);
    topBoxLayout->addWidget(addButtonLbl, Qt::AlignLeft);
    topBoxLayout->addStretch(1);
    topBoxLayout->addWidget(filterLbl);
    topBoxLayout->addWidget(addressDropdown);

    layout->addWidget(titleLbl);
    layout->addSpacing(10);
    layout->addWidget(topBox);
    layout->addSpacing(15);
    layout->addWidget(addressTbl);
    layout->addSpacing(20);

    fundsMenu = new BlocknetFundsMenu;
    fundsMenu->setDisplayWidget(popupWidget);
    fundsMenu->hOnSendFunds = [&]() { emit sendFunds(); };
    fundsMenu->hOnRequestFunds = [&]() { emit requestFunds(); };
    fundsMenu->hide();
}

void BlocknetAddressBook::setWalletModel(WalletModel *w) {
    if (walletModel == w)
        return;

    walletModel = w;
    if (!walletModel || !walletModel->getOptionsModel())
        return;

    //initialize();
    addressTbl->setWalletModel(walletModel);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookAvatar, QHeaderView::Fixed);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookAlias, QHeaderView::Stretch);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookAddress, QHeaderView::ResizeToContents);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookCopy, QHeaderView::Stretch);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookEdit, QHeaderView::Stretch);
    addressTbl->horizontalHeader()->setSectionResizeMode(BlocknetAddressBookFilterProxy::AddressBookDelete, QHeaderView::Stretch);
}

void BlocknetAddressBook::initialize() {
    if (!walletModel)
        return;

    dataModel.clear();

    Address ad1 = {
        tr("funds"),
        tr("MlgLuhfsFDJFjhabcscdIOJbdancacjttt")
    };
    Address ad2 = {
        tr("Nick Roman"),
        tr("KlrnuhfsFDJFjhabjnvkIOJbdancacnksd")
    };
    Address ad3 = {
        tr("Retirement"),
        tr("POKLuhfsFDJFjhabcscdIOJbdancacnksd")
    };
    Address ad4 = {
        tr("Zandaya Jordan"),
        tr("YUinuhfsFDJFjhabcscdIOJbdancavvxzl")
    };
    Address ad5 = {
        tr(""),
        tr("UUibHJkdslJFjhabcscdIOJbdancawpfnn")
    };

    dataModel << ad1 << ad2 << ad3 << ad4 << ad5;

    // Sort on alias descending
    std::sort(dataModel.begin(), dataModel.end(), [](const Address &a, const Address &b) {
        return a.alias > b.alias;
    });

    //this->setData(dataModel);

    //this->setModel(walletModel->getAddressTableModel());

    //addressTbl->setModel(walletModel->getAddressTableModel());
    //addressTbl->sortByColumn(0, Qt::AscendingOrder);
}

void BlocknetAddressBook::unwatch() {
    table->setEnabled(false);
}

void BlocknetAddressBook::watch() {
    table->setEnabled(true);
}

void BlocknetAddressBook::aliasChanged(const QString &alias) {
    addressTbl->setAlias(alias);
}

void BlocknetAddressBook::addressChanged(const QString &address) {
    addressTbl->setAddress(address);
}

QVector<BlocknetAddressBook::Address> BlocknetAddressBook::filtered(int filter, int chainHeight) {
    QVector<Address> r;
    for (auto &d : dataModel) {
        switch (filter) {
            case FILTER_SENDING: 
            case FILTER_RECEIVING: 
            case FILTER_ALL:
            default:
                r.push_back(d);
                break;
        }
    }
    return r;
}

void BlocknetAddressBook::setData(QVector<Address> data) {
    this->filteredData = data;

    unwatch();
    table->clearContents();
    table->setRowCount(this->filteredData.count());
    table->setSortingEnabled(false);

    for (int i = 0; i < this->filteredData.count(); ++i) {
        auto &d = this->filteredData[i];

        // action item
        auto *actionItem = new QTableWidgetItem;
        auto *widget = new QWidget();
        widget->setContentsMargins(QMargins());
        auto *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(QMargins());
        boxLayout->setSpacing(0);
        widget->setLayout(boxLayout);

        auto *button = new BlocknetActionBtn;
        button->setID(d.address);
        boxLayout->addWidget(button, 0, Qt::AlignCenter);
        connect(button, &BlocknetActionBtn::clicked, this, &BlocknetAddressBook::onAddressAction);

        table->setCellWidget(i, COLUMN_ACTION, widget);
        table->setItem(i, COLUMN_ACTION, actionItem);

        // avatar
        auto *avatarItem = new QTableWidgetItem;
        auto *avatarWidget = new QWidget();
        avatarWidget->setContentsMargins(QMargins());
        auto *avatarLayout = new QVBoxLayout;
        avatarLayout->setContentsMargins(QMargins());
        avatarLayout->setSpacing(0);
        avatarWidget->setLayout(avatarLayout);

        auto *avatar = new BlocknetAvatar(d.alias);
        avatarLayout->addWidget(avatar, 0, Qt::AlignCenter);
        
        table->setCellWidget(i, COLUMN_AVATAR, avatarWidget);
        table->setItem(i, COLUMN_AVATAR, avatarItem);

        // alias
        auto *aliasItem = new QTableWidgetItem;
        aliasItem->setData(Qt::DisplayRole, d.alias);
        table->setItem(i, COLUMN_ALIAS, aliasItem);

        // address
        auto *addressItem = new QTableWidgetItem;
        addressItem->setData(Qt::DisplayRole, d.address);
        table->setItem(i, COLUMN_ADDRESS, addressItem);

        // copy item
        auto *copyItem = new QTableWidgetItem;
        auto *copyWidget = new QWidget();
        copyWidget->setContentsMargins(QMargins());
        auto *copyLayout = new QVBoxLayout;
        copyLayout->setContentsMargins(QMargins());
        copyLayout->setSpacing(0);
        copyWidget->setLayout(copyLayout);

        auto *copyButton = new BlocknetLabelBtn;
        copyButton->setText(tr("Copy Address"));
        //copyButton->setFixedSize(40, 40);
        copyButton->setID(d.address);
        copyLayout->addWidget(copyButton, 0, Qt::AlignCenter);
        copyLayout->addSpacing(6);
        //connect(copyButton, &BlocknetLabelBtn::clicked, this, &BlocknetAddressBook::onAddressAction);

        table->setCellWidget(i, COLUMN_COPY, copyWidget);
        table->setItem(i, COLUMN_COPY, copyItem);

        // edit item
        auto *editItem = new QTableWidgetItem;
        auto *editWidget = new QWidget();
        editWidget->setContentsMargins(QMargins());
        auto *editLayout = new QVBoxLayout;
        editLayout->setContentsMargins(QMargins());
        editLayout->setSpacing(0);
        editWidget->setLayout(editLayout);

        auto *editButton = new BlocknetLabelBtn;
        editButton->setText(tr("Edit"));
        //editButton->setFixedSize(40, 40);
        editButton->setID(d.address);
        editLayout->addWidget(editButton, 0, Qt::AlignCenter);
        editLayout->addSpacing(6);
        //connect(editButton, &BlocknetLabelBtn::clicked, this, &BlocknetAddressBook::onAddressAction);

        table->setCellWidget(i, COLUMN_EDIT, editWidget);
        table->setItem(i, COLUMN_EDIT, editItem);

        // delete item
        auto *deleteItem = new QTableWidgetItem;
        auto *deleteWidget = new QWidget();
        deleteWidget->setContentsMargins(QMargins());
        auto *deleteLayout = new QVBoxLayout;
        deleteLayout->setContentsMargins(QMargins());
        deleteLayout->setSpacing(0);
        deleteWidget->setLayout(deleteLayout);

        auto *deleteButton = new BlocknetLabelBtn;
        deleteButton->setText(tr("Delete"));
        //deleteButton->setFixedSize(40, 40);
        deleteButton->setID(d.address);
        deleteLayout->addWidget(deleteButton, 0, Qt::AlignCenter);
        deleteLayout->addSpacing(6);
        //connect(deleteButton, &BlocknetLabelBtn::clicked, this, &BlocknetAddressBook::onAddressAction);

        table->setCellWidget(i, COLUMN_DELETE, deleteWidget);
        table->setItem(i, COLUMN_DELETE, deleteItem);
    }

    table->setSortingEnabled(true);
    watch();
}

void BlocknetAddressBook::onAddressAction() {
    auto *btn = qobject_cast<BlocknetActionBtn*>(sender());
    auto addressHash = uint256S(btn->getID().toStdString());
    if (fundsMenu->isHidden()) {
        QPoint li = btn->mapToGlobal(QPoint());
        QPoint npos = popupWidget->mapFromGlobal(QPoint(li.x() - 2, li.y() + btn->height() + 2));
        fundsMenu->move(npos);
        fundsMenu->show();
    }
}

BlocknetAddressBookTable::BlocknetAddressBookTable(QWidget *parent) : QTableView(parent),
                                                                                    walletModel(nullptr) {
}

void BlocknetAddressBookTable::setWalletModel(WalletModel *w) {
    if (walletModel == w)
        return;
    walletModel = w;

    if (walletModel == nullptr) {
        setModel(nullptr);
        return;
    }

    //this->setItemDelegateForColumn(BlocknetAddressBookFilterProxy::AddressBookAction, new BlocknetAddressBookCellItem(this));

    // Set up transaction list
    auto *filter = new BlocknetAddressBookFilterProxy(walletModel->getOptionsModel(), this);
    filter->setSourceModel(walletModel->getAddressTableModel());
    filter->setDynamicSortFilter(true);
    filter->setSortRole(Qt::EditRole);
    filter->setFilterRole(Qt::EditRole);
    filter->setSortCaseSensitivity(Qt::CaseInsensitive);
    filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filter->sort(BlocknetAddressBookFilterProxy::AddressBookAlias, Qt::DescendingOrder);
    setModel(filter);

    //setModel(walletModel->getAddressTableModel());
}

void BlocknetAddressBookTable::leave() {
    this->blockSignals(true);
    model()->blockSignals(true);
}
void BlocknetAddressBookTable::enter() {
    this->blockSignals(false);
    model()->blockSignals(false);
}

void BlocknetAddressBookTable::setAlias(const QString &alias) {
    auto *m = dynamic_cast<BlocknetAddressBookFilterProxy*>(this->model());
    m->setAlias(alias);
}

void BlocknetAddressBookTable::setAddress(const QString &address) {
    auto *m = dynamic_cast<BlocknetAddressBookFilterProxy*>(this->model());
    m->setAddress(address);
}

BlocknetAddressBookFilterProxy::BlocknetAddressBookFilterProxy(OptionsModel *o, QObject *parent) : QSortFilterProxyModel(parent),
                                                                                                                 optionsModel(o),
                                                                                                                 limitRows(-1),
                                                                                                                 alias(QString()),
                                                                                                                 address(QString()) {}

void BlocknetAddressBookFilterProxy::setAlias(const QString &alias) {
    this->alias = alias;
    invalidateFilter();
}

void BlocknetAddressBookFilterProxy::setAddress(const QString &address) {
    this->address = address;
    invalidateFilter();
}

bool BlocknetAddressBookFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    return true;
}

bool BlocknetAddressBookFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    switch (left.column()) {
        case AddressBookAvatar: {
            auto l1 = sourceModel()->index(left.row(), AddressTableModel::Label).data(Qt::DisplayRole);
            auto r1 = sourceModel()->index(right.row(), AddressTableModel::Label).data(Qt::DisplayRole);
            return l1.toLongLong() < r1.toLongLong();
        }
        case AddressBookAlias: {
            auto l2 = sourceModel()->index(left.row(), AddressTableModel::Label).data(Qt::DisplayRole);
            auto r2 = sourceModel()->index(right.row(), AddressTableModel::Label).data(Qt::DisplayRole);
            return l2.toString() < r2.toString();
        }
        case AddressBookAddress: {
            auto l3 = sourceModel()->index(left.row(), AddressTableModel::Address).data(Qt::DisplayRole);
            auto r3 = sourceModel()->index(right.row(), AddressTableModel::Address).data(Qt::DisplayRole);
            return l3.toString() < r3.toString();
        }
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

int BlocknetAddressBookFilterProxy::columnCount(const QModelIndex &) const {
    return BlocknetAddressBookFilterProxy::AddressBookDelete + 1;
}

int BlocknetAddressBookFilterProxy::rowCount(const QModelIndex& parent) const {
    if (limitRows != -1)
        return std::min(QSortFilterProxyModel::rowCount(parent), limitRows);
    else
        return QSortFilterProxyModel::rowCount(parent);
}

QVariant BlocknetAddressBookFilterProxy::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case AddressBookAction:
                    return tr("");
                case AddressBookAvatar:
                    return tr("");
                case AddressBookAlias:
                    return tr("Alias");
                case AddressBookAddress:
                    return tr("Address");
                case AddressBookCopy:
                    return tr("");
                case AddressBookEdit:
                    return tr("");
                case AddressBookDelete:
                    return tr("");
                default:
                    return tr("");
            }
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft;
        }
    }
    return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant BlocknetAddressBookFilterProxy::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto *model = dynamic_cast<AddressTableModel*>(sourceModel());
    QModelIndex sourceIndex = mapToSource(index);
    
    //auto *rec = static_cast<TransactionRecord*>(model->index(sourceIndex.row(), 0).internalPointer());
    //auto* rec = static_cast<AddressTableEntry*>(model->index(sourceIndex.row(), 0).internalPointer());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case AddressBookAction:
                return tr("action");//model->data(sourceIndex, role);
            case AddressBookAvatar:
                return tr("avatar");//model->data(sourceIndex, role);
            case AddressBookAlias:
                return tr("alias");
            case AddressBookAddress:
                return tr("address");//model->data(model->index(sourceIndex.row(), 1, sourceIndex.parent()), role);
            case AddressBookCopy:
                return tr("copy");
            case AddressBookEdit:
                return tr("edit");
            case AddressBookDelete:
                return tr("delete");
        }
    }
    return QVariant();
   // return QSortFilterProxyModel::data(index, role);
}

void BlocknetAddressBook::setModel(AddressTableModel* model)
{
    this->model = model;
    if (!model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    /*switch (tab) {
    case ReceivingTab:
        // Receive filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Receive);
        break;
    case SendingTab:
        // Send filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Send);
        break;
    }*/

    addressTbl->setModel(proxyModel);
    addressTbl->sortByColumn(BlocknetAddressBookFilterProxy::AddressBookAlias, Qt::AscendingOrder);

    //selectionChanged();
}

BlocknetAddressBookCellItem::BlocknetAddressBookCellItem(QObject *parent) : QStyledItemDelegate(parent) { }

void BlocknetAddressBookCellItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    /*if (index.column() == BlocknetAddressBookFilterProxy::AddressBookAction) {
        //painter->save();

        auto *widget = new QWidget();
        widget->setContentsMargins(QMargins());
        auto *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(QMargins());
        boxLayout->setSpacing(0);
        widget->setLayout(boxLayout);

        auto *button = new BlocknetActionBtn;
        //auto *address = static_cast<QString>(index.data(Qt::DisplayRole));
        //button->setID(address);
        boxLayout->addWidget(button, 0, Qt::AlignCenter);
        //connect(button, &BlocknetActionBtn::clicked, this, &BlocknetAddressBook::onAddressAction);

        QStyleOptionComplex box;
        box.rect = option.rect;

        //QWidget *srcWidget = qobject_cast<QWidget *>(option.styleObject);
        //// style->metaObject()->className() = QStyleSheetStyle
        //QStyle *style = srcWidget ? srcWidget->style() : QApplication::style();

        //style->drawComplexControl(QStyle::CC_CustomBase, &box, painter, widget);
        QApplication::style()->drawComplexControl(QStyle::CC_CustomBase, &box, painter, widget);
        //QApplication::style()->drawComplexControl(QStyle::CC)
        //QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &box, painter, 0);
    
        //painter->restore();
    } else {
         QStyledItemDelegate::paint(painter, option, index);
    }*/
    //QStyledItemDelegate::paint(painter, option, index);
}

QWidget *BlocknetAddressBookCellItem::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {

    return QStyledItemDelegate::createEditor(parent, option, index);

    /*if (index.column() == BlocknetAddressBookFilterProxy::AddressBookAction) {
        auto *widget = new QWidget();
        widget->setContentsMargins(QMargins());
        auto *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(QMargins());
        boxLayout->setSpacing(0);
        widget->setLayout(boxLayout);

        auto *button = new BlocknetActionBtn;
        //auto *address = static_cast<QString>(index.data(Qt::DisplayRole));
        //button->setID(address);
        boxLayout->addWidget(button, 0, Qt::AlignCenter);
        //connect(button, &BlocknetActionBtn::clicked, this, &BlocknetAddressBook::onAddressAction);
        return widget;
    } else {
        auto *widget = new QWidget();
        widget->setContentsMargins(QMargins());
        auto *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(QMargins());
        boxLayout->setSpacing(0);
        widget->setLayout(boxLayout);

        auto *button = new BlocknetActionBtn;
        //auto *address = static_cast<QString>(index.data(Qt::DisplayRole));
        //button->setID(address);
        boxLayout->addWidget(button, 0, Qt::AlignCenter);
        //connect(button, &BlocknetActionBtn::clicked, this, &BlocknetAddressBook::onAddressAction);
        return widget;
        //return QStyledItemDelegate::createEditor(parent, option, index);
    }*/
}

QSize BlocknetAddressBookCellItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == BlocknetAddressBookFilterProxy::AddressBookAction)
        return {50, option.rect.height()};
    return QStyledItemDelegate::sizeHint(option, index);
}
