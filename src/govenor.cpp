
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


#include <QtWidgets/QApplication>
#include <QSystemTrayIcon>

#include "govenor.h"


ConnectionGovenor_t::ConnectionGovenor_t( HumbleLogin_t* login, HumbleNetwork_t* network, HumbleUserData_t* userdata, HumbleMainWin_t* mainwin ) :
    QDialog(),
    _hlogin( login ),
    _hnetwork( network ),
    _huserdata( userdata ),
    _hmainwin( mainwin )
{
    //FIXME:
    QSystemTrayIcon* systray = new QSystemTrayIcon( QIcon(":/humble/h_big.png") );
    systray->show();

    //Create status window (loggin in .. fetching orders with progress bar)
    QVBoxLayout* layout = new QVBoxLayout( this );

    _progressbar = new QProgressBar( this );   
    _statuslabel = new QLabel( this );

    layout->addWidget( _statuslabel );
    layout->addWidget( _progressbar );

    layout->setSizeConstraint( QLayout::SetFixedSize );
    setLayout( layout );

    //Set Captcha to login widget when found
    connect( _hnetwork, &HumbleNetwork_t::captchaSought, [=] ( QPixmap captcha, QString c ){
        _hlogin->setCaptcha( captcha );
        _challenge = c;
        setCurrentState( CREDENTIALS );
    });

    //Login when user entered credentials
    connect( _hlogin, &HumbleLogin_t::credentialsReady, [=](QString user,QString pw,QString captcha) {
        setCurrentState(LOGGING_IN);
        _hnetwork->login( user,pw,captcha,_challenge );
    });

    //Re-open Loginwindow when logged out for whatever reason
    connect( _hnetwork, &HumbleNetwork_t::loginStateChanged, [=] (bool logged_in) {
        if (logged_in)  
            setCurrentState( FETCHING_USERDATA );
        else {
            _hnetwork->seekCaptcha();
        }
    });

    //Update progressbar when orders were downloaded
    connect( _huserdata, &HumbleUserData_t::orderReceived, [=] (int n, int total) {
        _progressbar->setRange(0,total);
        _progressbar->setValue(n);

        if (n == total) setCurrentState( READY );
    });
}

void ConnectionGovenor_t::setCurrentState( ApplicationState state )
{
    switch (state)
    {
    case CREDENTIALS:
        _huserdata->setUser("");
        _statuslabel->setText( "" );
        _hlogin->show();       
        hide(); 
        _hmainwin->hide();
        break;
    case LOGGING_IN:
        _statuslabel->setText( tr("Connecting as") + "<strong> " + _hlogin->username() + "</strong>..." );
        show();
        _hlogin->hide();
        _hmainwin->hide();
        _progressbar->setRange(0,0);
        break;
    case FETCHING_USERDATA:
        _huserdata->requestUserData(); //FIXME: offline mode?

        _statuslabel->setText( tr("Retreiving order(s) for") + "<strong> " + _hlogin->username() + "</strong>..." );
        show();
        _hlogin->hide();
        _hmainwin->hide();
        _progressbar->setRange(0,0);
        break;
    case READY: 
        _statuslabel->setText( "" );
        _hmainwin->show();
        _hlogin->hide();
        hide();
        break;
    };
}


void ConnectionGovenor_t::rule() {
    setCurrentState(CREDENTIALS);
}


