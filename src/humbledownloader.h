
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


#ifndef HUMBLEDOWNLOADER_H
#define HUMBLEDOWNLOADER_H

#include <QObject>
#include <QMap>

#include "abstractdownloadprotocol.h"

class HumbleDownloader  : public QObject
{
    Q_OBJECT

public:
    static HumbleDownloader& get();

    template<typename T>
    typename std::enable_if< std::is_base_of<AbstractDownloadProtocol,T>::value >::type 
        addProtocol( ) 
    {   
        T* obj = new T;
        AbstractDownloadProtocol* prot = qobject_cast<AbstractDownloadProtocol*>(obj);
        
        if   (prot != nullptr ) registerProtocol( prot );
        else qDebug("Unable to register download protocol.");
    }

    //TODO: settings: download location! Do not allow changing download location while there is an active dl. Store dlid with gamekey,subp,url,platform,location
    int enqueue( QString gamekey, QString subproduct, QString platform, QString protocol, QUrl url );

    QWidget* downloadIndicatorWidget() const;
    QWidget* downloadQueueWidget()     const;

public slots:
    void dispatchFinishedDownload( AbstractDownloadProtocol*, dlid_t );

signals:
//    void downloadStarted( AbstractDownloadProtocol* loader, AbstractDownloadReply );
// loader->createDetailedWidget();
// loader->createProgressBar();
//    void downloadPaused( AbstractDownloadProtocol* loader, AbstractDownloadReply );
//    void downloadStopped( AbstractDownloadProtocol* loader, AbstractDownloadReply );

private:
    HumbleDownloader();
    ~HumbleDownloader();

    void registerProtocol( AbstractDownloadProtocol* );

    QMap<QString,AbstractDownloadProtocol*> m_downloader;
};


#endif
