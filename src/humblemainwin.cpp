
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

#include "humblemainwin.h"
#include "humbledownloader.h"

HumbleMainWin_t::HumbleMainWin_t(  ) : QDialog()
{
    QVBoxLayout* main_layout = new QVBoxLayout( this );

    _browser = new SimpleBrowser_t();   

    _details = new GameDetailsWidget_t;
    _liblist = new LibraryListWidget_t;

    //navbar
    QHBoxLayout* navbar_layout = new QHBoxLayout();
    _hb_button  = new QPushButton( tr("HumbleBundle.com") );
    _lib_button = new QPushButton( tr("Library") );
    _usr_button = new QPushButton( HumbleUserData_t::instance().user() );

    //library & Details
    QVBoxLayout* library_layout= new QVBoxLayout();
    _library      = new QWidget;
    _lib_splitter = new QSplitter( Qt::Horizontal );
    

    //Settings 
    QHBoxLayout* hbox_settings = new QHBoxLayout;
    QPushButton* pb_settings   = new QPushButton;

    //Downloads
    _details_splitter = new QSplitter( Qt::Vertical );


    //Navigation-bar: Humble-homepage, Library, User Preferences + optional library header
    _hb_button->setFlat( true );
    _lib_button->setFlat(true );
    _usr_button->setFlat(true );

    navbar_layout->addWidget( _hb_button );
    navbar_layout->addWidget( _lib_button );
    navbar_layout->addWidget( _usr_button );

    main_layout->addLayout( navbar_layout );
    main_layout->addWidget( _library );
    main_layout->addWidget( _browser );

    setLayout( main_layout );

    //Settings
    hbox_settings->addSpacerItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding) );
    hbox_settings->addWidget( pb_settings );
    pb_settings->setCheckable( true );

    //Library & Details & Downloads
    QWidget*     mainwidget = new QWidget;
    QVBoxLayout* mainlayout = new QVBoxLayout;
    mainwidget->setLayout( mainlayout );
    
    mainlayout->addLayout( hbox_settings,1 );
    mainlayout->addWidget( _details,10000000 );
    mainlayout->addWidget( HumbleDownloader::get().downloadIndicatorWidget() );

    _details_splitter->addWidget( mainwidget );
    _details_splitter->addWidget( HumbleDownloader::get().downloadQueueWidget() );

    _lib_splitter->addWidget( _liblist );
    _lib_splitter->addWidget( _details_splitter  );

    library_layout->addWidget( _lib_splitter );
    _library->setLayout( library_layout );

    //Reload browser in background when user changes
    connect( &HumbleNetwork_t::instance(), &HumbleNetwork_t::loginStateChanged, [=] (bool logged_in) {
        if (logged_in) _browser->load( HumbleNetwork_t::urlRoot() );
    });

    //Interaction with liblist:
    connect( &HumbleUserData_t::instance(),   &HumbleUserData_t::iconChanged,    _liblist, &LibraryListWidget_t::setIcon );
    connect( _liblist, &LibraryListWidget_t::productSelected, this, &HumbleMainWin_t::showProductDetails );

    //Navigation buttons let specific widgets show and hide
    connect( _hb_button, &QPushButton::clicked, this, &HumbleMainWin_t::navigateBrowser );
    connect( _lib_button,&QPushButton::clicked, this, &HumbleMainWin_t::navigateLibrary );

    connect( &HumbleUserData_t::instance(), &HumbleUserData_t::orderReceived, [=] (int n, int total) {
        if (n == total)  {
            _liblist->updateLibrary();
            _usr_button->setText( HumbleUserData_t::instance().user() );
        }
    });

    //Settings:
    //TODO

    navigateLibrary();
}


void HumbleMainWin_t::navigateBrowser()
{
    _browser->show();
    _library->hide();
}


void HumbleMainWin_t::navigateLibrary()
{
    _browser->hide();
    _library->show();
}


void HumbleMainWin_t::showProductDetails( QString gamekey, QString machine_name )
{
    QString filter = _liblist->currentFilter();

    if ( !gamekey.isNull() && !machine_name.isNull() )  //Show details of an subproduct
    {
        QJsonDocument subproduct = HumbleUserData_t::instance().subproduct( gamekey, machine_name );
        _details->setSubproduct( subproduct,filter );
    }
    else
        if ( !gamekey.isNull() && machine_name.isNull() ) //Show details of an order
        {
            QJsonDocument order = HumbleUserData_t::instance().order( gamekey );
            _details->setOrder( order );
         }
}

