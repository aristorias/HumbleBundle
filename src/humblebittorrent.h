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


#ifndef HUMBLEBITTORRENT_H
#define HUMBLEBITTORRENT_H

#include <QWidget>
#include <QMap>

#include "abstractdownloadprotocol.h"
#include "bittorrent/torrentclient.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QProgressBar;
QT_END_NAMESPACE
class HumbleBittorrent_t;
class TorrentClient;
class SubproductIconLabel_t;

class BittorrentDownloadWidget_t : public QWidget
{
    Q_OBJECT

    dlid_t m_id;
    HumbleBittorrent_t* m_bt;

    SubproductIconLabel_t* m_icon;
    QLabel*   m_l_title;
    QLabel*   m_l_platform;
    QLabel*   m_l_peerinfo;
    QLabel*   m_l_up;
    QLabel*   m_l_down;
    
    QPushButton* m_pb_pause;
    QPushButton* m_pb_close;

    QProgressBar* m_pbar;

public:
    BittorrentDownloadWidget_t( HumbleBittorrent_t*, dlid_t );

public slots:
    void torrentError( TorrentClient::Error );
    void torrentStopped();
    void updatePeerInfo();
    void updateDownloadRate( int );
    void updateUploadRate  ( int );

};

class HumbleBittorrent_t : public AbstractDownloadProtocol
{
public:
    struct Job_t
    {
        TorrentClient* client;

        QString gamekey;
        QString machine_name;
        QString platform;
        QString url_str;
    };

    HumbleBittorrent_t();

    dlid_t resume( QUrl url, QString gamekey, QString machine_name, QString platform );
    QWidget* widget( dlid_t id );

    QString  name()     const;
    Job_t job( dlid_t ) const;

    void start( );
    void stop( dlid_t );
    bool togglePause( dlid_t );

private:
    QMap<dlid_t,Job_t> m_jobs;
};

#endif
