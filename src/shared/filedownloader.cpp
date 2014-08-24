
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


#include <QNetworkReply>
#include <QEventLoop>
#include <QFile>

#include "filedownloader.h"

QNetworkRequest::Attribute FileDownloader_t::TimeStartedAttribute = QNetworkRequest::Attribute( QNetworkRequest::User + 1 );
QNetworkRequest::Attribute FileDownloader_t::FileDumpAttribute    = QNetworkRequest::Attribute( QNetworkRequest::User + 2 );
QNetworkRequest::Attribute FileDownloader_t::RedirectionsAttribute= QNetworkRequest::Attribute( QNetworkRequest::User + 3 );

FileDownloader_t::FileDownloader_t()
{
    m_nam = new QNetworkAccessManager;
    m_max_redirections = 0;
    m_max_parallel = 1;
}

void FileDownloader_t::setAutoRedirect( int p_max ) {
    m_max_redirections = p_max;
}

int FileDownloader_t::autoRedirect() const {
    return m_max_redirections;
}

void FileDownloader_t::setParallelDownloads( int p_max ) {
    m_max_parallel = p_max;
}

int FileDownloader_t::parallelDownloads() const {
    return m_max_parallel;
}


QNetworkRequest& FileDownloader_t::prepend( const QNetworkRequest& p_request )
{
    m_requests.push_front( p_request );
    return m_requests.last();
}

QNetworkRequest& FileDownloader_t::prepend( const QUrl& p_url ) 
{
    QNetworkRequest request( p_url );
    return prepend(request);
}

QNetworkRequest& FileDownloader_t::append( const QNetworkRequest& p_request )
{
    m_requests.push_back( p_request );
    return m_requests.last();
}

QNetworkRequest& FileDownloader_t::append( const QUrl& p_url ) 
{
    QNetworkRequest request( p_url );
    return append(request);
}


QNetworkRequest& FileDownloader_t::resume( const QUrl& p_url, qint64 p_size )
{
    QByteArray header = "bytes=" + QByteArray::number(p_size) + "-";
    
    QNetworkRequest* request = &append( p_url );
    request->setRawHeader( "Range", header );

    return *request;
}


QNetworkRequest& FileDownloader_t::resume( const QUrl& p_url, QString filename )
{
    qint64 size = 0;

    if ( QFile::exists(filename) )
    {
        QFile file( filename );
        file.open( QIODevice::ReadOnly );
        size = file.size();
        file.close();
    }

    //TODO: use qiodevice and store it as attribute. . . 

    QNetworkRequest* request = &resume( p_url,size );
    request->setAttribute( FileDumpAttribute,filename );
    return *request;
}


QNetworkReply* FileDownloader_t::startOne()
{
    if (m_replies.size() >= parallelDownloads() && parallelDownloads() != 0) return nullptr;

    if (m_requests.isEmpty()) {
        if (m_replies.isEmpty()) emit finished();
        return nullptr;
    }

    QNetworkRequest request = m_requests.first();
    request.setAttribute( TimeStartedAttribute, QTime::currentTime() );
    m_requests.pop_front();

    QNetworkReply* reply = m_nam->get( request );
    m_replies.push_back( reply );

    connect( reply, &QNetworkReply::downloadProgress, [=] ( qint64 p_BytesReceived, qint64 p_BytesTotal ) 
    {
        int status    = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        if (status != 206 ) return;

        QTime time = reply->request().attribute( TimeStartedAttribute ).toTime();
        int    sec = time.elapsed()/1000;
        qint64 kb  = p_BytesReceived/1024;    

        double speed = double(kb)/sec;

        emit downloadProgress( reply, p_BytesReceived, p_BytesTotal, speed );
    } );

    connect( reply, &QNetworkReply::finished, [=] () {
        int status    = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        int redirects = reply->request().attribute( RedirectionsAttribute ).toInt();

        m_replies.removeAll( reply );

        if ( status == 302 && autoRedirect() > redirects ) 
        {
            QNetworkRequest request = reply->request();
            request.setAttribute( RedirectionsAttribute, ++redirects );
            request.setUrl( reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl() );    
            prepend( request );
            reply->deleteLater();
        }
 
//        startOne();      
        if ( m_requests.isEmpty() && m_replies.isEmpty() ) emit finished();

        emit downloadFinished( reply );
    } );

    QString filedump = request.attribute( FileDumpAttribute ).toString();
    if (!filedump.isEmpty()) //FIXME: Files are being created even when redirected...
    {
        connect( reply, &QNetworkReply::readyRead, [=] () {
            int status    = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if (status == 206) {
                QFile file(filedump);
                if (file.open( QIODevice::WriteOnly | QIODevice::Append )) 
                    file.write( reply->readAll() );
                file.close();
            }
        } );

        connect( reply, &QNetworkReply::finished, [=] () {  
            int status    = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
            if (status == 206) {
                QFile file(filedump);
                if (file.open( QIODevice::WriteOnly | QIODevice::Append )) 
                    file.write( reply->readAll() );
                file.close();
            }
        } );
    }

    emit downloadStarted( reply );
    return reply;
}


void FileDownloader_t::start()
{
    for (int i=0; i<parallelDownloads(); ++i) startOne();   
}

void FileDownloader_t::wait() const
{
    QEventLoop waiter;

    connect( this, &FileDownloader_t::finished, &waiter, &QEventLoop::quit );

    if (!m_replies.isEmpty()) waiter.exec();
}

