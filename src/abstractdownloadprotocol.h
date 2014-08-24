/*
 * HumbleBundle.com Desktop client to manage, download, install and use your purchases.
 * https://github.com/aristorias/HumbleBundle
 * Copyright (C) 2014 Samuel Schmitz
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ABSTRACTDOWNLOADPROTOCOL_H
#define ABSTRACTDOWNLOADPROTOCOL_H

#include <QObject>

//typedef unsigned dlid_t; //No QSignalMapper Compatibility :( should be really enough tough ;)
typedef int dlid_t;

class QWidget;

class AbstractDownloadProtocol  : public QObject
{
    Q_OBJECT

    static QList<dlid_t> m_used_ids;

public:
    AbstractDownloadProtocol();
    virtual ~AbstractDownloadProtocol();

    virtual dlid_t resume( QUrl url, QString gamekey, QString machine_name, QString platform ) = 0;

    virtual QWidget* widget( dlid_t id ) = 0;
    virtual QString  name()        const = 0;

    dlid_t takeId();
    void releaseId( dlid_t );

public slots:
    virtual void start( ) = 0;
    virtual void stop( dlid_t ) = 0;
    virtual bool togglePause( dlid_t ) = 0;

signals:
    void downloadEnqueued( AbstractDownloadProtocol* protocol, dlid_t id );
    void downloadStarted( AbstractDownloadProtocol* protocol, dlid_t id );
    void downloadProgress( AbstractDownloadProtocol* protocol, dlid_t id, qint64 rec, qint64 tot, double speed );
    void downloadFinished( AbstractDownloadProtocol* protocol, dlid_t id );
};


#endif
