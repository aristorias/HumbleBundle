
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

#include "humbledownloader.h"


HumbleDownloader& HumbleDownloader::get()
{
    static HumbleDownloader downloader;
    return downloader;
}


HumbleDownloader::HumbleDownloader() : QObject() {}

HumbleDownloader::~HumbleDownloader() 
{
    for ( auto name : m_downloader.keys() ) {
        delete m_downloader[name];
        m_downloader[name] = nullptr;
    }
}


void HumbleDownloader::registerProtocol( AbstractDownloadProtocol* protocol ) 
{
    m_downloader[protocol->name()]  = protocol;

    connect( protocol, &AbstractDownloadProtocol::downloadFinished, this, &HumbleDownloader::dispatchFinishedDownload );
}

void HumbleDownloader::dispatchFinishedDownload( AbstractDownloadProtocol* protocol, dlid_t id )
{
    QCryptographicHash md5( QCryptographicHash::Md5 );
    QCryptographicHash sha1( QCryptographicHash::Sha1 );

    QFile downed_file;
    //TODO: open+failcheck

    md5.addData( dynamic_cast<QIODevice*>(&downed_file) );

    downed_file.reset();
    sha1.addData( dynamic_cast<QIODevice*>(&downed_file) );   

    //TODO: checksum with PROGRESS!
    //TODO: emit signal, systray, gamelist probably want to indicate there is something to install
    //TODO: show error, emit error signal when cheksums are not valid...
}


int HumbleDownloader::enqueue( QString gamekey, QString subproduct, QString platform, QString protocol, QUrl url )
{
    int id = 0;

    if ( m_downloader.contains(protocol) ) 
    {
        id = m_downloader[protocol]->resume( url,gamekey,subproduct,platform );
    }
    else
        QMessageBox::information( 0,"Not implemented",QString("Downloads via %1 are not implemented (yet).").arg(protocol) );

    return id;
}


QWidget* HumbleDownloader::downloadIndicatorWidget() const
{   
    QProgressBar* widget = new QProgressBar;
    widget->setRange(0,100);

    return widget;
}

QWidget* HumbleDownloader::downloadQueueWidget() const
{
    QWidget*     widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout*  hbox   = new QHBoxLayout;
    QLabel*       uplabel= new QLabel( tr("Upload:") );
    QLabel*     downlabel= new QLabel( tr("Download:") );
    QSpinBox*     upspeed = new QSpinBox;
    QSpinBox*   downspeed = new QSpinBox;

    QListWidget* listwidget = new QListWidget;

    
    for ( auto name : m_downloader.keys() )
    {
        AbstractDownloadProtocol* protocol = m_downloader[name];

        connect( protocol, &AbstractDownloadProtocol::downloadEnqueued, [=] ( AbstractDownloadProtocol* protocol, dlid_t id )
        {
            QWidget* item_widget = protocol->widget(id);

            QListWidgetItem* item = new QListWidgetItem;
            item->setSizeHint( QSize(80,80) ); //FIXME: guessed size
            listwidget->addItem( item );
            listwidget->setItemWidget( item,item_widget );

            connect( item_widget, &QObject::destroyed, [=] ( QObject* ) {
                delete item;
            } );

        } );

    }

    upspeed->setSuffix  ( " KB/s" );
    downspeed->setSuffix( " KB/s" );


    hbox->addWidget( downlabel );
    hbox->addWidget( downspeed );
    hbox->addSpacerItem( new QSpacerItem( 0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );
    hbox->addWidget( uplabel );
    hbox->addWidget( upspeed );
    hbox->setSizeConstraint( QLayout::SetMinimumSize );

    layout->addWidget( listwidget,10000000 );
    layout->addLayout( hbox,1 );

    widget->setLayout( layout );

    return widget;
}


