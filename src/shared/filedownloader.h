
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


#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QTimer>
#include <QNetworkRequest>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class FileDownloader_t : public QObject
{
    Q_OBJECT

    struct Job_t 
    {
        QNetworkRequest request;
        QNetworkReply*  reply;
    };

    QTimer  m_timer;

    QList<QNetworkRequest>  m_requests;
    QList<QNetworkReply*>   m_replies;
    QNetworkAccessManager*  m_nam;

    int m_max_redirections;
    int m_max_parallel;

public:
    static QNetworkRequest::Attribute TimeStartedAttribute;
    static QNetworkRequest::Attribute FileDumpAttribute;
    static QNetworkRequest::Attribute RedirectionsAttribute;

    FileDownloader_t();

    void setAutoRedirect( int );
    int  autoRedirect() const; 

    void setParallelDownloads( int );
    int  parallelDownloads() const;

    QNetworkRequest& append( const QNetworkRequest& url ); 
    QNetworkRequest& append( const QUrl& url ); //TODO: add userdata //TODO: do not allow overriding QTimer userdata
    QNetworkRequest& resume( const QUrl& url, qint64 size );
    QNetworkRequest& resume( const QUrl& url, QString filename );

    //TODO: download speed
    //TODO: what happens if user aborts download?
    QNetworkReply* startOne();
    void start();
    void wait() const;

    QList<QNetworkRequest>& queue() { return m_requests; }
    QList<QNetworkRequest>  queue() const { return m_requests; }
    
signals:
    void downloadStarted( QNetworkReply* );
    void downloadProgress( QNetworkReply*, qint64 received, qint64 total, double speed ); //FIXME: use delta(milliseconds) instead of speed for greate flexibility
    void downloadFinished( QNetworkReply* );

    void finished();

private: 
    //Private: Redirect request will be prepended. Can conflict with users prepending manually and downloaded lastly..
    QNetworkRequest& prepend( const QNetworkRequest& url ); 
    QNetworkRequest& prepend( const QUrl& url ); 
};


#endif
