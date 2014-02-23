#include "clicommandsql.h"
#include "cli.h"
#include "parser/ast/sqliteselect.h"
#include "parser/parser.h"
#include "parser/parsererror.h"
#include "db/queryexecutor.h"
#include "db/sqlresults.h"
#include "qio.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QList>

CliCommandSql *CliCommandSql::create()
{
    return new CliCommandSql();
}

void CliCommandSql::execute(QStringList args)
{
    Db* db = cli->getCurrentDb();
    if (!db || !db->isOpen())
    {
        println("Database is not open.");
        return;
    }

    // Executor deletes itself later when called with lambda.
    QueryExecutor *executor = new QueryExecutor(db, args[0]);

    executor->exec([=](SqlResultsPtr results)
    {
        if (results->isError())
        {
            qOut << "Error " << results->getErrorCode() << ": " << results->getErrorText() << "\n";
            qOut.flush();
            return;
        }

        quint32 maxLength = cli->getMaxColLength();

        // Columns
        foreach (QString colName, results->getColumnNames())
            qOut << colName.left(maxLength) << "|";

        qOut << "\n";
        qOut.flush();

        // Data
        SqlResultsRowPtr row;
        while (!(row = results->next()).isNull())
        {
            foreach (QVariant value, row->valueList())
                qOut << value.toString().left(maxLength) << "|";

            qOut << "\n";
            qOut.flush();
        }
    });
}

bool CliCommandSql::validate(QStringList args)
{
    if (args.size() != 1)
        return false;

    if (!cli->getCurrentDb())
    {
        println("No working database is set.");
        println("Call .use command to set working database.");
        println("Call .dblist to see list of all databases.");
        return false;
    }

    return true;
}