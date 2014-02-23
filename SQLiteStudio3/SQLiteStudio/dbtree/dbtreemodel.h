#ifndef DBTREEMODEL_H
#define DBTREEMODEL_H

#include "db/db.h"
#include "dbtreeitem.h"
#include "config.h"
#include <QStandardItemModel>
#include <QObject>

class DbManager;
class DbTreeView;
class DbPlugin;

class DbTreeModel : public QStandardItemModel
{
    Q_OBJECT

    public:
        DbTreeModel();
        ~DbTreeModel();

        void connectDbManagerSignals();
        DbTreeItem* findItem(DbTreeItem::Type type, const QString &name);
        DbTreeItem* findItem(DbTreeItem::Type type, Db* db);
        QList<DbTreeItem*> findItems(DbTreeItem::Type type);
        void move(QStandardItem* itemToMove, QStandardItem* newParentItem, int newRow = -1);
        void move(QStandardItem* itemToMove, int newRow);
        DbTreeItem *createGroup(const QString& name, QStandardItem *parent = nullptr);
        void deleteGroup(QStandardItem* groupItem);
        QStandardItem *root() const;
        QStringList getGroupFor(QStandardItem* item);
        void storeGroups();
        void refreshSchema(Db* db);
        QList<DbTreeItem*> getAllItemsAsFlatList() const;
        void setTreeView(DbTreeView *value);
        QVariant data(const QModelIndex &index, int role) const;
        QStringList mimeTypes() const;
        QMimeData* mimeData(const QModelIndexList &indexes) const;

        static DbTreeItem* findItem(QStandardItem *parentItem, DbTreeItem::Type type, const QString &name);
        static DbTreeItem* findItem(QStandardItem* parentItem, DbTreeItem::Type type, Db* db);
        static QList<DbTreeItem*> findItems(QStandardItem* parentItem, DbTreeItem::Type type);

        static const constexpr char* MIMETYPE = "application/x-sqlitestudio-dbtreeitem";

    private:
        void readGroups(QList<Db*> dbList);
        QList<Config::DbGroupPtr> childsToConfig(QStandardItem* item);
        void restoreGroup(const Config::DbGroupPtr& group, QList<Db*>* dbList = nullptr, QStandardItem *parent = nullptr);
        bool applyFilter(QStandardItem* parentItem, const QString& filter);
        void refreshSchema(Db* db, QStandardItem* item);
        void collectExpandedState(QHash<QString, bool>& state, QStandardItem* parentItem = nullptr);
        QStandardItem* refreshSchemaDb(Db* db);
        QList<QStandardItem*> refreshSchemaTables(const QStringList &tables, bool sort);
        QHash<QString, QList<QStandardItem *> > refreshSchemaTableColumns(const QHash<QString, QStringList> &columns);
        QMap<QString,QList<QStandardItem*> > refreshSchemaIndexes(const QMap<QString, QStringList> &indexes, bool sort);
        QMap<QString,QList<QStandardItem*> > refreshSchemaTriggers(const QMap<QString, QStringList> &triggers, bool sort);
        QList<QStandardItem*> refreshSchemaViews(const QStringList &views, bool sort);
        void populateChildItemsWithDb(QStandardItem* parentItem, Db* db);
        void refreshSchemaBuild(QStandardItem* dbItem, QList<QStandardItem*> tables, QMap<QString,QList<QStandardItem*> > indexes,
                                QMap<QString,QList<QStandardItem*> > triggers, QList<QStandardItem*> views, QHash<QString,
                                QList<QStandardItem *> > allTableColumns);
        void restoreExpandedState(const QHash<QString, bool>& expandedState, QStandardItem* parentItem);
        QString getToolTip(DbTreeItem *item) const;
        QString getDbToolTip(DbTreeItem *item) const;
        QString getInvalidDbToolTip(DbTreeItem *item) const;
        QString getTableToolTip(DbTreeItem *item) const;
        QList<DbTreeItem*> getChildsAsFlatList(QStandardItem* item) const;

        static const QString toolTipTableTmp;
        static const QString toolTipHdrRowTmp;
        static const QString toolTipRowTmp;
        static const QString toolTipIconRowTmp;

        DbTreeView* treeView;

    private slots:
        void expanded(const QModelIndex &index);
        void collapsed(const QModelIndex &index);
        void dbAdded(Db* db);
        void dbUpdated(const QString &oldName, Db* db);
        void dbRemoved(Db* db);
        void dbConnected(Db* db);
        void dbDisconnected(Db* db);
        void dbToBeUnloaded(Db* db, DbPlugin* plugin);
        void dbLoaded(Db* db, DbPlugin* plugin);

    public slots:
        void loadDbList();
        void itemChangedVisibility(DbTreeItem* item);
        void applyFilter(const QString& filter);

    signals:
        void updateItemHidden(DbTreeItem* item);
};

#endif // DBTREEMODEL_H