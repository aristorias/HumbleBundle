
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

#include <iostream>

#include "govenor.h"
#include "humbledownloader.h"
#include "humblebittorrent.h"
#include "humbleweb.h"

using namespace std;

int main( int argc, char** argv )
{
    QApplication app(argc,argv);
    app.setWindowIcon( QIcon(QPixmap(":/humble/h_big.png")) ); 

    //TODO: QCommandLineParser (username, password, offline)
       
    QCoreApplication::setApplicationName( "HumbleBundle Desktop Client" );
    QCoreApplication::setOrganizationName( QUrl(HumbleNetwork_t::urlRoot()).host() );

    HumbleDownloader::get().addProtocol<HumbleWeb_t>();
    HumbleDownloader::get().addProtocol<HumbleBittorrent_t>();

    HumbleLogin_t    humbleLogin;
    HumbleMainWin_t  humbleApp;
 

    ConnectionGovenor_t govenor( &humbleLogin, &HumbleNetwork_t::instance(), &HumbleUserData_t::instance(), &humbleApp );
    govenor.rule();

    return app.exec();
}
