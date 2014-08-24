
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



#include "gamedetailswidget.h"
#include "widgets/spiconlabel.h"
#include "widgets/pficonlabel.h"

#include "humbleuserdata.h"
#include "humbledownloader.h"

QWidget* createSubproductWidget( const QJsonDocument& json )
{
    QString gamekey     = json.object()["gamekey"].toString();
    QString subproduct  = json.object()["machine_name"].toString();

    QHBoxLayout* hlayout = new QHBoxLayout;
    QVBoxLayout* vlayout = new QVBoxLayout;
    QWidget* widget      = new QWidget;

    SubproductIconLabel_t* icon_label    = new SubproductIconLabel_t(gamekey,subproduct);
    QLabel* title_label   = new QLabel;
    QLabel* payee_label   = new QLabel;

    QHBoxLayout* hbox_services = new QHBoxLayout;
    QJsonObject meta_data_game = json.object()["meta_data"].toObject()["content"].toObject()["game"].toObject();
    for ( auto service : meta_data_game.keys() )
    {
        QStringList platforms;
        QJsonArray json_platforms = meta_data_game[service].toArray();
        
        for ( auto platform : json_platforms ) platforms << platform.toString();

        hbox_services->addWidget( new PlatformIconLabel_t(service,platforms) );
    }
    hbox_services->addSpacerItem( new QSpacerItem( 0,0,QSizePolicy::Expanding,QSizePolicy::Expanding ) );
    hbox_services->setContentsMargins( 0,0,0,0 );


    hlayout->setContentsMargins( 0,0,0,0 );
    hlayout->addWidget( icon_label );
    hlayout->addLayout( vlayout );
    vlayout->addWidget( title_label );
    vlayout->addWidget( payee_label );
    vlayout->addLayout( hbox_services );
    vlayout->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );

    widget->setLayout( hlayout );

    QString human_name = json.object()["human_name"].toString();
    QString payee_name = json.object()["payee"].toObject()["human_name"].toString();
    //TODO: link human_name with url
    QString payee_url  = json.object()["url"].toString(); //FIXME: use meta-data->developer url

    //TODO: use platformiconlabel

    QFont title_font;
    title_font.setBold( true );
    //title_font.setPointSize( title_font.pointSize()*2 );

    title_label->setText( human_name );
    title_label->setFont( title_font );
    title_label->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    payee_label->setText( QString("<a href=\"%1\">%2</a>").arg(payee_url).arg(payee_name) );
    payee_label->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    widget->setMaximumHeight( icon_label->height() );
    return widget;
}


QWidget* createOrderWidget( const QJsonDocument& json )
{
    QWidget* widget = new QWidget;
    widget->setLayout( new QHBoxLayout );
    
    QString human_name   = json.object()["product"].toObject()["human_name"].toString();
    double  familyamount = json.object()["familyamount"].toDouble();

    QLabel* title_label = new QLabel;
    title_label->setText( QString("<strong>%1</strong><br />%2$").arg(human_name).arg(familyamount) );

    widget->layout()->addWidget( title_label );

    return widget;
}


GameDetailsWidget_t::GameDetailsWidget_t() : QScrollArea()
{
    QWidget* widget = new QWidget;
    setWidgetResizable( true );
    setWidget( widget );
}


void GameDetailsWidget_t::setSubproduct( const QJsonDocument& doc, QString platform ) //FIXME: function to switch platform: create all, but hide filtered entries!
{
   //Delete old content and create new layout
    for ( auto child : widget()->children() ) delete child;
    delete widget()->layout();

    //Create new content
    QWidget*     header_widget = createSubproductWidget( doc );

    QString sp_human_name = doc.object()["human_name"].toString();
    QString sp_payee_name = doc.object()["payee"].toObject()["human_name"].toString();

    QVBoxLayout* box_layout    = new QVBoxLayout;
    QGroupBox*   downloads_box = new QGroupBox;
    downloads_box->setLayout( box_layout );
    downloads_box->setTitle( tr("Available download(s) and transfer protocol(s):") );

    //Add new content
    for ( auto dl : doc.object()["downloads"].toArray() ) {

        if ( dl.toObject()["platform"].toString() != platform && platform != "all" ) continue;

        for ( auto dl_struct : dl.toObject()["download_struct"].toArray() ) {
            
            if ( dl_struct.toObject()["message"].isNull() ) //FIXME: allow message and downloads in parallel add fat32 warning and bittorrent recommendation!
            {
                QGroupBox*          dl_box = new QGroupBox( dl_struct.toObject()["name"].toString() );
                QHBoxLayout* dl_box_layout = new QHBoxLayout;
                dl_box->setAlignment( Qt::AlignHCenter );
                dl_box->setLayout( dl_box_layout );

                QJsonObject dl = dl_struct.toObject()["url"].toObject();
                for ( auto dl_type : dl.keys() )
                {
                    QIcon        dl_icon( QString(":/platforms/%1.png").arg(dl_type) ); //TODO: use QPixmapCache?
                    QPushButton* dl_button = new QPushButton( );

                    dl_button->setFlat(true);
                    dl_button->setText( dl_type );
                    dl_button->setIcon( dl_icon );
                    dl_box_layout->addWidget( dl_button );
    
                    connect( dl_button, &QPushButton::clicked, [=] () { 
                        QString gamekey    = doc.object()["gamekey"].toString();
                        QString subproduct = doc.object()["machine_name"].toString();
                        QString protocol   = dl_type;
                        QString url        = dl[dl_type].toString();

                        HumbleDownloader::get().enqueue( gamekey,subproduct,platform,protocol,url );
                    } );
                }

                downloads_box->layout()->addWidget( dl_box );
            }
            else {
                QLabel* message_label = new QLabel( dl_struct.toObject()["message"].toString() );
                downloads_box->layout()->addWidget( message_label );
            }
        }
    }
    widget()->setLayout( new QVBoxLayout );
    widget()->layout()->addWidget( header_widget );
    widget()->layout()->addWidget( downloads_box );
    widget()->layout()->setSizeConstraint( QLayout::SetFixedSize );
}


void GameDetailsWidget_t::setOrder( const QJsonDocument& doc )
{
    //Delete old content and create new layout
    for ( auto child : widget()->children() ) delete child;
    delete widget()->layout();

    QString gamekey        = doc.object()["gamekey"].toString();

    QGroupBox*   sp_box     = new QGroupBox;
    QVBoxLayout* box_layout = new QVBoxLayout;
    sp_box->setLayout( box_layout );
    sp_box->setTitle( tr("Products") );

    for ( auto subproduct : doc.object()["subproducts"].toArray() )
    {
        QJsonObject obj = subproduct.toObject();
        obj["gamekey"]  = gamekey;
        box_layout->addWidget( createSubproductWidget(QJsonDocument(obj)) );
    }

    widget()->setLayout( new QVBoxLayout );
    widget()->layout()->addWidget( createOrderWidget(doc) );
    widget()->layout()->addWidget( sp_box );
    widget()->layout()->setSizeConstraint( QLayout::SetFixedSize );
}


