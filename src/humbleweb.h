
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


#ifndef HUMBLEWEB_H
#define HUMBLEWEB_H

#include "shared/filedownloader.h"
#include "abstractdownloadprotocol.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QProgressBar;
class QPushButton;
QT_END_NAMESPACE;
class HumbleWeb_t;
class SubproductIconLabel_t;
class AbstractDownloadProtocol;


class WebDownloaderWidget_t : public QWidget
{
    Q_OBJECT

    dlid_t m_id;

    QPushButton* m_pb_pause;
    QPushButton* m_pb_close;

    QLabel*  m_l_title;
    QLabel*  m_l_platform;
    QLabel*  m_l_progress;
    SubproductIconLabel_t* m_icon;

    QProgressBar* m_pbar;

public:
    WebDownloaderWidget_t(HumbleWeb_t* web, dlid_t id);

public slots:
    void downloadProgress( AbstractDownloadProtocol*,dlid_t,qint64,qint64,double );
    void downloadFinished( AbstractDownloadProtocol*,dlid_t );
};



class HumbleWeb_t : public AbstractDownloadProtocol
{
    FileDownloader_t m_webloader;

    QMap<dlid_t,QNetworkRequest>  m_id_request;
    QMap<dlid_t,QNetworkReply*>   m_id_reply;

public:
    HumbleWeb_t();

    void  start();
    bool  togglePause( dlid_t ); 
    void  stop( dlid_t ); //Remove from queue or abort running download

    dlid_t resume( QUrl, QString gamekey, QString machine_name, QString platform );
 
    QNetworkRequest id2Request( dlid_t );

    QWidget* widget( dlid_t id );
    QString  name()         const; 

};

#endif
