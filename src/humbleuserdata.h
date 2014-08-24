
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


#ifndef HUMBLEUSERDATA_H
#define HUMBLEUSERDATA_H

#include <QJsonDocument>
#include <QPixmap>

#include "humblenetwork.h"

class HumbleUserData_t : public QObject
{
    Q_OBJECT

    HumbleNetwork_t* _hnet;

    QString _user;

    QJsonDocument _user_order;
    QJsonDocument _order;

public:
    enum RequestType {
        API_ORDER = 1,
        API_USER_ORDER,
        ICON_REQUEST,
        OTHER
    };
    
    static HumbleUserData_t& instance();

    void setUser( QString username );
    QString user() const { return _user; }
    QList<QString> knownUsers() const;

    void requestUserData( RequestType = API_USER_ORDER, QString gamekey = "", QString machine_name = "" );

    QJsonDocument userOrder() const; 
    QJsonDocument order( QString gamekey = QString() ) const; 
    QJsonDocument subproduct( QString gamekey, QString machine_name ) const;

    //Tries to load to disk an then fetch from internet
    QJsonDocument getJson( QString machine_name = "" );
    QPixmap       getIcon( QString gamekey, QString machine_name ); 
 
    //Save/Load to/from disk:
    void saveJson( ) const;
    void saveIcon( const QPixmap&, QString, QString );

   
    static QString saveRootPath();
    static QString saveIconPath();
           QString saveUserPath() const;
           QString saveJsonPath() const;

public slots:
    void receiveData( QNetworkReply* );

signals:    
    void orderReceived( int n, int total );
    void iconChanged( QPixmap pix, QString gamekey, QString machine_name );

private:
    HumbleUserData_t();
    ~HumbleUserData_t();
};

#endif
