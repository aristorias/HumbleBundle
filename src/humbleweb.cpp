
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


#include <QtWidgets>

#include "humbleuserdata.h"
#include "humblenetwork.h"
#include "humbleweb.h"
#include "widgets/spiconlabel.h"

WebDownloaderWidget_t::WebDownloaderWidget_t( HumbleWeb_t* web, dlid_t id  ) : m_id(id)
{
    //Get some basic information about the download
    QNetworkRequest request = web->id2Request( id );

    QString gamekey     = request.attribute( HumbleNetwork_t::HUMBLE_GAMEKEY ).toString();
    QString machine_name= request.attribute( HumbleNetwork_t::HUMBLE_MACHINE_NAME ).toString();
    QString platform    = request.attribute( HumbleNetwork_t::PLATFORM ).toString();
    QString dlsize = "";

    //Retreive some information about the product
    QJsonDocument json = HumbleUserData_t::instance().subproduct( gamekey,machine_name );

    for (auto dl : json.object()["downloads"].toArray() )
        if ( dl.toObject()["platform"] == platform )
            for ( auto dl_struct : dl.toObject()["download_struct"].toArray() )
                if (dl_struct.toObject()["url"].toObject()[web->name()] == request.url().toDisplayString())
                    dlsize = dl_struct.toObject()["human_size"].toString();


    //Allocate Widgets and Layouts
    QHBoxLayout* hbox = new QHBoxLayout;
    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* hbox_line1 = new QHBoxLayout;
    QHBoxLayout* hbox_line2 = new QHBoxLayout;
    QHBoxLayout* hbox_line3 = new QHBoxLayout;

    m_icon  = new SubproductIconLabel_t(gamekey,machine_name);
    m_l_title = new QLabel( json.object()["human_name"].toString() );

    m_l_progress = new QLabel;
    m_l_progress->setText( dlsize );

    m_l_platform = new QLabel;
    m_l_platform->setPixmap( QPixmap(QString(":/platforms/%1.png").arg(platform)) );

    m_pbar  = new QProgressBar;
    m_pbar->setRange( 0,100 );

    m_pb_pause = new QPushButton("Start");
    m_pb_close = new QPushButton("Close");

    //Bring it all together
    hbox_line1->addWidget( m_l_platform );
    hbox_line1->addWidget( m_l_title);
    hbox_line1->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );

    hbox_line2->addWidget( m_pb_pause );
    hbox_line2->addWidget( m_pb_close );
    hbox_line2->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );

    hbox_line3->addWidget( m_l_progress );
    hbox_line3->addWidget( m_pbar );

    vbox->addLayout( hbox_line1 );
    vbox->addLayout( hbox_line2 );
    vbox->addLayout( hbox_line3 );

    hbox->addWidget( m_icon );
    hbox->addLayout( vbox );

    setLayout( hbox );

    //Add signal+slot functionality
    connect( web, &AbstractDownloadProtocol::downloadProgress,  this, &WebDownloaderWidget_t::downloadProgress  );
    connect( web, &AbstractDownloadProtocol::downloadFinished,  this, &WebDownloaderWidget_t::downloadFinished  );
    connect( m_pb_pause, &QPushButton::clicked, [=] () { (web->togglePause(m_id)) ? m_pb_pause->setText(tr("Start")) : m_pb_pause->setText(tr("Pause")); } );
    connect( m_pb_close, &QPushButton::clicked, [=] () { web->stop(m_id); deleteLater(); } );
}


void WebDownloaderWidget_t::downloadFinished( AbstractDownloadProtocol* protocol, dlid_t id )
{
    Q_UNUSED(protocol);
    if (m_id != id) return;

    m_pb_pause->setEnabled( false );
}

void WebDownloaderWidget_t::downloadProgress( AbstractDownloadProtocol* protocol, dlid_t id, qint64 rec, qint64 tot, double speed )
{
    Q_UNUSED( protocol );
    if (m_id != id) return;

    m_pbar->setValue( double(rec)/tot*100. );

    float r = int(rec/1024./1024.*100)/100.;
    float t = int(tot/1024./1024.*100)/100.;
    m_l_progress->setText( tr("%1%2 of %3%4").arg(r).arg("mb").arg(t).arg("mb") );
}


HumbleWeb_t::HumbleWeb_t() : AbstractDownloadProtocol()
{
//    m_webloader.setParallelDownloads(0);

    FileDownloader_t::TimeStartedAttribute = HumbleNetwork_t::TIMESTARTED;
    FileDownloader_t::FileDumpAttribute    = HumbleNetwork_t::FILEDUMP;
    FileDownloader_t::RedirectionsAttribute= HumbleNetwork_t::NREDIRECTIONS;

    connect( &m_webloader, &FileDownloader_t::downloadStarted, [=] ( QNetworkReply* reply ) {
        dlid_t id = reply->request().attribute( QNetworkRequest::Attribute(HumbleNetwork_t::DOWNLOAD_ID) ).value<dlid_t>();       
        m_id_request.remove( id );
        m_id_reply[id] = reply;

        emit downloadStarted( this,id );
    } );
    
    connect( &m_webloader, &FileDownloader_t::downloadProgress, [=] ( QNetworkReply* reply, qint64 rec, qint64 total, double speed ) {
        dlid_t id = reply->request().attribute( HumbleNetwork_t::DOWNLOAD_ID ).value<dlid_t>();

        emit downloadProgress( this,id,rec,total,speed );
        if ( rec==total ) emit downloadFinished( this,id );
    } );

    connect( &m_webloader, &FileDownloader_t::downloadFinished, [=] ( QNetworkReply* reply ) {
        dlid_t id = reply->request().attribute( HumbleNetwork_t::DOWNLOAD_ID ).value<dlid_t>();
        m_id_reply.remove( id );
        releaseId( id );
        reply->deleteLater();
    } );
}


void HumbleWeb_t::start()
{
//    m_webloader.start();
}



dlid_t HumbleWeb_t::resume( QUrl url, QString gamekey, QString machine_name, QString platform )
{
    dlid_t id = takeId();

    //FIXME: download location == cwd?
    QNetworkRequest* request = &m_webloader.resume( url,url.fileName() );
    request->setAttribute( HumbleNetwork_t::HUMBLE_GAMEKEY,QVariant(gamekey) );
    request->setAttribute( HumbleNetwork_t::HUMBLE_MACHINE_NAME,QVariant(machine_name) );
    request->setAttribute( HumbleNetwork_t::PLATFORM,QVariant(platform) );
    request->setAttribute( HumbleNetwork_t::DOWNLOAD_ID,QVariant(id) );

    m_id_request[id] = *request;
    emit downloadEnqueued( this,id );

    return id;
}


void HumbleWeb_t::stop( dlid_t id )
{
    if ( m_id_reply.contains(id) )
    {
        m_id_reply[id]->abort();
    }
    else
        if ( m_id_request.contains(id) )
        {
            m_webloader.queue().removeOne( m_id_request[id] );
            m_id_request.remove(id);
            releaseId( id );
        }
}

bool HumbleWeb_t::togglePause( dlid_t id )
{
    bool paused = true;

    if ( m_id_reply.contains(id) )
    {
        QNetworkReply*  reply   = m_id_reply[id];
        QNetworkRequest request = reply->request();

        reply->abort();
        m_id_request[id] = request;
        m_webloader.queue().push_front( request );
    }
    else
        if ( m_id_request.contains(id) )
        {
            QNetworkRequest request = m_id_request[id];

            if( m_webloader.queue().removeOne( request ) ) {
                m_webloader.queue().push_front( request );
                if (m_webloader.startOne()) 
                    paused = false;
                else
                    qDebug() << "HumbleWeb_t::togglePause: could not start download" << id;
            }
            else
                qWarning() << "HumbleWeb_t::togglePause: unable to sort queue";
        }

    return paused;
}

QNetworkRequest HumbleWeb_t::id2Request( dlid_t id )
{
    QNetworkRequest request;
    if ( m_id_request.contains(id) ) request = m_id_request[id];
    else
        if ( m_id_reply.contains(id) ) request = m_id_reply[id]->request();
    return request;
}


QWidget* HumbleWeb_t::widget( dlid_t id ) 
{
    if ( id2Request(id) == QNetworkRequest() ) return nullptr;
    return new WebDownloaderWidget_t( this,id );
}


QString HumbleWeb_t::name() const {
    return "web";
}

