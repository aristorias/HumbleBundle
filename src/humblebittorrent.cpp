
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


#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QtWidgets>

#include "humblebittorrent.h"
#include "humbleuserdata.h"
#include "widgets/spiconlabel.h"
#include "bittorrent/torrentclient.h"
#include "bittorrent/ratecontroller.h"


BittorrentDownloadWidget_t::BittorrentDownloadWidget_t( HumbleBittorrent_t* bt, dlid_t id )
{
    m_id = id;
    m_bt = bt;

    HumbleBittorrent_t::Job_t job = bt->job(id);
    QJsonDocument json = HumbleUserData_t::instance().subproduct( job.gamekey,job.machine_name );

    //Allocate widgets
    m_icon       = new SubproductIconLabel_t( job.gamekey,job.machine_name );
    m_l_platform = new QLabel;
    m_l_title    = new QLabel;
    m_l_peerinfo = new QLabel;
    m_l_up       = new QLabel( tr("U: 0 KB/s") );
    m_l_down     = new QLabel( tr("D: 0 KB/s") );
    
    m_pb_pause = new QPushButton( tr("Start") );
    m_pb_close = new QPushButton( tr("Close") );
    m_pbar     = new QProgressBar;

    QHBoxLayout* hbox = new QHBoxLayout;
    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* vbox_line1 = new QHBoxLayout;
    QHBoxLayout* vbox_line2 = new QHBoxLayout;
    QHBoxLayout* vbox_line3 = new QHBoxLayout;

    //Configure widgets
    m_pbar->setRange(0,100);
    m_l_title->setText( json.object()["human_name"].toString() );
    m_l_platform->setPixmap( QString(":/platforms/%1.png").arg(job.platform) );

    //Bring it all together
    vbox_line1->addWidget( m_l_platform );
    vbox_line1->addWidget( m_l_title );
    vbox_line1->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );

    vbox_line2->addWidget( m_pb_pause );
    vbox_line2->addWidget( m_pb_close );
    vbox_line2->addWidget( m_l_peerinfo );
    vbox_line2->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );

    vbox_line3->addWidget( m_l_down );
    vbox_line3->addWidget( m_l_up );
    vbox_line3->addWidget( m_pbar );

    vbox->addLayout( vbox_line1 );
    vbox->addLayout( vbox_line2 );
    vbox->addLayout( vbox_line3 );

    hbox->addWidget( m_icon );
    hbox->addLayout( vbox );

    setLayout( hbox );

    //Add functionality
    connect( job.client, &TorrentClient::stateChanged, [=] (TorrentClient::State) { m_pb_pause->setText(job.client->stateString()); } );
    connect( job.client, SIGNAL(progressUpdated(int)), m_pbar, SLOT(setValue(int)) );
    connect( job.client, SIGNAL(error(TorrentClient::Error)), this, SLOT(torrentError(TorrentClient::Error)) );
    connect( job.client, SIGNAL(peerInfoUpdated()), this, SLOT(updatePeerInfo()) );
    connect( job.client, SIGNAL(downloadRateUpdated(int)), this, SLOT(updateDownloadRate(int)) );
    connect( job.client, SIGNAL(uploadRateUpdated(int)), this, SLOT(updateUploadRate(int)) );
    connect( job.client, SIGNAL(stopped()), this, SLOT(torrentStopped()) );
    connect( m_pb_pause, &QPushButton::clicked, [=] () { bt->togglePause(id); } );
    connect( m_pb_close, &QPushButton::clicked, [=] () { bt->stop(id); deleteLater();} );


}


void BittorrentDownloadWidget_t::torrentError( TorrentClient::Error error )
{
    qDebug() << QString("Error while downloaing %1: %2").arg(m_l_title->text()).arg(m_bt->job(m_id).client->errorString()); 
}


void BittorrentDownloadWidget_t::updatePeerInfo()
{
    HumbleBittorrent_t::Job_t job = m_bt->job(m_id);
    m_l_peerinfo->setText( QString("Peers/Seed: %1/%2").arg(job.client->connectedPeerCount()).arg(job.client->seedCount()) );
}


void BittorrentDownloadWidget_t::updateDownloadRate(int bytesPerSec) 
{
    HumbleBittorrent_t::Job_t job = m_bt->job(m_id);

    float down = int(bytesPerSec/1024.*100)/100.;
    m_l_down->setText( QString("D: %1 KB/s").arg(down) );
}

void BittorrentDownloadWidget_t::updateUploadRate(int bytesPerSec) 
{
    HumbleBittorrent_t::Job_t job = m_bt->job(m_id);

    float down = int(bytesPerSec/1024.*100)/100.;
    m_l_up->setText( QString("U: %1 KB/s").arg(down) );
}

void BittorrentDownloadWidget_t::torrentStopped() 
{
    m_pb_pause->setText( tr("Start") );
}


HumbleBittorrent_t::HumbleBittorrent_t()
{
    int rate = 500;
    RateController::instance()->setDownloadLimit(rate * 1024);
    RateController::instance()->setUploadLimit(rate * 1024);
}

dlid_t HumbleBittorrent_t::resume( QUrl url, QString gamekey, QString machine_name, QString platform )
{
    QNetworkAccessManager network;
    QNetworkRequest request( url );
    QNetworkReply* reply = network.get( request );
    reply->setReadBufferSize(0);

    QEventLoop waiter;
    connect( &network,&QNetworkAccessManager::finished,&waiter,&QEventLoop::quit );
    waiter.exec();

    TorrentClient* client = new TorrentClient;
    if( !client->setTorrent( reply->readAll() ) )
    {
        //FIXME: error handling
        qWarning() << "Unable to fetch torrent:" << reply->request().url().toDisplayString();
        delete client;  
        client = nullptr;
        return 0;
    }
    client->setDestinationFolder("/tmp/");
    //client->setDumpedState( QByteArray() ); //FIXME
    client->setDumpedState( 0 ); //FIXME

    Job_t job = { client,gamekey,machine_name,platform,url.toDisplayString() };

    dlid_t id  = takeId();
    m_jobs[id] = job;
 
    emit downloadEnqueued( this,id );

    return id;
}


void HumbleBittorrent_t::start()
{
}

bool HumbleBittorrent_t::togglePause( dlid_t id )
{
    bool pause = true;

    if ( m_jobs.contains(id) )
    {
        Job_t job = m_jobs[id];
    
        if ( job.client->state() == TorrentClient::Idle )
        {
            pause = false;
            job.client->start();
        } 
        else
        {
            pause = job.client->state() != TorrentClient::Paused;
            job.client->setPaused( pause );
        }
    }

    return pause;
}

void HumbleBittorrent_t::stop( dlid_t id )
{
    if ( m_jobs.contains(id) )
    {
        Job_t job = m_jobs[id];
        job.client->stop();

        delete job.client;
        m_jobs.remove(id);
    }
}


QWidget* HumbleBittorrent_t::widget( dlid_t id )
{
    if (!m_jobs.contains(id)) return nullptr;

    return new BittorrentDownloadWidget_t( this,id );
}

HumbleBittorrent_t::Job_t HumbleBittorrent_t::job( dlid_t id ) const
{
    Job_t job;
    if (m_jobs.contains(id)) job = m_jobs[id];
    return job;
}

QString HumbleBittorrent_t::name() const
{
    return "bittorrent";
}

