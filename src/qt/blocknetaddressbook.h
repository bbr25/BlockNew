// Copyright (c) 2018 The Blocknet Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BLOCKNETADDRESSBOOK_H
#define BLOCKNETADDRESSBOOK_H

#include "walletmodel.h"
#include "blocknetdropdown.h"
#include "blocknetactionbtn.h"
#include "blocknetfundsmenu.h"
#include "optionsmodel.h"
#include "addresstablemodel.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QVariant>
#include <QModelIndex>
#include <QPainter>

class BlocknetAddressBookTable;

class BlocknetAddressBook : public QFrame 
{
    Q_OBJECT

public:
    explicit BlocknetAddressBook(QWidget *popup, QFrame *parent = nullptr);
    void setWalletModel(WalletModel *w);

    struct Address {
        QString alias;
        QString address;
    };

private:
    WalletModel *walletModel;
    QVBoxLayout *layout;
    QLabel *titleLbl;
    QLabel *addButtonLbl;
    QLabel *filterLbl;
    BlocknetDropdown *addressDropdown;
    BlocknetAddressBookTable *addressTbl;
    AddressTableModel* model;
    QSortFilterProxyModel* proxyModel;
    QVector<Address> dataModel;
    QVector<Address> filteredData;
    BlocknetFundsMenu *fundsMenu = nullptr;
    QWidget *popupWidget;

    void setData(QVector<Address> data);
    QVector<Address> filtered(int filter, int chainHeight);
    void unwatch();
    void watch();

    enum {
        COLUMN_ACTION,
        COLUMN_AVATAR,
        COLUMN_ALIAS,
        COLUMN_ADDRESS,
        COLUMN_COPY,
        COLUMN_EDIT,
        COLUMN_DELETE
    };

    enum {
        FILTER_ALL,
        FILTER_SENDING,
        FILTER_RECEIVING
    };

public slots:
    void onAddressAction();
    void aliasChanged(const QString &alias);
    void addressChanged(const QString &address);

signals:
    void sendFunds();
    void requestFunds();
};

class BlocknetAddressBookTable : public QTableView {
    Q_OBJECT

public:
    explicit BlocknetAddressBookTable(QWidget *parent = nullptr);
    void setWalletModel(WalletModel *w);
    void setAlias(const QString &alias);
    void setAddress(const QString &address);
    void leave();
    void enter();

private:
    WalletModel *walletModel;
};

class BlocknetAddressBookFilterProxy : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit BlocknetAddressBookFilterProxy(OptionsModel *o, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &) const override;

    static const QDateTime MIN_DATE; // Earliest date that can be represented (far in the past)
    static const QDateTime MAX_DATE; // Last date that can be represented (far in the future)
    static const quint32 ALL_ADDRESSES = 0;
    static const quint32 SENDING = 1;
    static const quint32 RECEIVING = 2;
    static quint32 TYPE(int type) { return 1 << type; }

    enum ColumnIndex {
        AddressBookAction = 0,
        AddressBookAvatar = 1,
        AddressBookAlias = 2,
        AddressBookAddress = 3,
        AddressBookCopy = 4,
        AddressBookEdit = 5,
        AddressBookDelete = 6
    };

    void setLimit(int limit); // Set maximum number of rows returned, -1 if unlimited.
    void setAlias(const QString &alias);
    void setAddress(const QString &address);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    OptionsModel *optionsModel;
    int limitRows;
    QString alias;
    QString address;
};

#include <QStyledItemDelegate>

/**
 * @brief Responsible for drawing the custom table columns (Status, Date, Amount).
 * @param o
 * @param parent
 */
class BlocknetAddressBookCellItem : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit BlocknetAddressBookCellItem(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // BLOCKNETADDRESSBOOK_H
