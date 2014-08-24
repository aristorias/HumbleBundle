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

#ifndef GOVENOR_H
#define GOVENOR_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QProgressBar>

#include "humblelogin.h"
#include "humblenetwork.h"
#include "humbleuserdata.h"
#include "humblemainwin.h"


class ConnectionGovenor_t : public QDialog
{
    Q_OBJECT

    QProgressBar*      _progressbar;
    QLabel*            _statuslabel;

    QString            _challenge; //captcha

    HumbleLogin_t*     _hlogin;
    HumbleNetwork_t*   _hnetwork;
    HumbleUserData_t*  _huserdata;
    HumbleMainWin_t*   _hmainwin;

    
public:
    enum ApplicationState {
        CREDENTIALS = 1,
        LOGGING_IN  = 2,
        FETCHING_USERDATA = 3,
        READY = 4
    };

    ConnectionGovenor_t( HumbleLogin_t* login, HumbleNetwork_t* network, HumbleUserData_t* userdat, HumbleMainWin_t* mainwin );

    void setCurrentState( ApplicationState );

    void rule();
};



#endif
