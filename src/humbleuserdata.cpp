
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


#include <QtCore>
#include <QPixmapCache>

#include "humbleuserdata.h"

HumbleUserData_t& HumbleUserData_t::instance() 
{
    static HumbleUserData_t userdata;
    return userdata;
}

HumbleUserData_t::HumbleUserData_t() : QObject()
{
    connect( &HumbleNetwork_t::instance(), &HumbleNetwork_t::humbleRequestFinished, this, &HumbleUserData_t::receiveData );
}

HumbleUserData_t::~HumbleUserData_t() {
    saveJson();
}

QString HumbleUserData_t::saveRootPath() {
    return QStandardPaths::writableLocation( QStandardPaths::DataLocation ) + QDir::separator();

}

QString HumbleUserData_t::saveIconPath() {
    return saveRootPath() + "icon" + QDir::separator();
}

QString HumbleUserData_t::saveUserPath() const {
    return saveRootPath() + "user" + QDir::separator() + _user  + QDir::separator();
}

QString HumbleUserData_t::saveJsonPath() const {
    return saveUserPath() + "json" + QDir::separator();
}

QJsonDocument HumbleUserData_t::userOrder() const {
    return _user_order;
}

QJsonDocument HumbleUserData_t::order( QString gamekey ) const {
    QJsonDocument order = _order;
    if ( !gamekey.isNull() ) {
        order = QJsonDocument(); //Return nothing if gamekey is invalid
        for ( auto o : _order.array() ) 
            if ( o.toObject()["gamekey"] == gamekey ) {
                order.setObject( o.toObject() );
                break;
            }
    }

    return order;
}


QJsonDocument HumbleUserData_t::subproduct( QString gamekey, QString machine_name ) const 
{
    QJsonDocument doc;

    for ( auto o : _order.array() )
        if ( o.toObject()["gamekey"] == gamekey ) {
            for ( auto s : o.toObject()["subproducts"].toArray() ) 
                if ( s.toObject()["machine_name"] == machine_name ) 
                {
                    //carrying gamekey information is useful when you want to pass Doc without additonal information. Can be identified be receiver (gamesdetailswidget::setSubproduct ;))  
                    QJsonObject object = s.toObject();
                    object.insert("gamekey",gamekey);
                    doc.setObject( object );
                    break;
                }
            break;
        }

    return doc;
}


QPixmap HumbleUserData_t::getIcon( QString gamekey, QString machine_name )
{
    QPixmap icon(":/tags/drmfree.svg"); 

    QString key             = gamekey + "_" + machine_name;
    QString filepath_prefix = saveIconPath() + key;

    if ( !QPixmapCache::find(key,&icon) )
    {
        QPixmap loaded;
        if ( loaded.load( saveIconPath() + key + ".png", "PNG") ) {
            icon = loaded;
            QPixmapCache::insert( key,icon );
        }
    }
    //TODO: Offline mode
    //FIXME: do not fetch it every time!! Potential endless loop.
    /* We *could* catch all icons at once but mainwindow can request icons too leading to double fetched icons at startup. */
    requestUserData( ICON_REQUEST, gamekey, machine_name );

    return icon;
}


void HumbleUserData_t::saveIcon( const QPixmap& pix, QString gamekey, QString machine_name ) 
{
    QString key = gamekey + "_" + machine_name;

    bool changed = false;

    QPixmap pix_cache;
    if (!QPixmapCache::find(key,&pix_cache)) 
        changed = true;
    else
        if (pix_cache.cacheKey() != pix.cacheKey())
            changed = true;

    if (changed) {
        QPixmapCache::insert( key, pix );

        QDir dir( saveIconPath() );
        if (!dir.exists())
            dir.mkpath( saveIconPath() );

        if (!pix.save( saveIconPath()+key+".png", "PNG" )) 
            qDebug() << "Failed to save pixmap:" << saveIconPath() + key;

        emit iconChanged( pix, gamekey, machine_name );
    }
}


QJsonDocument HumbleUserData_t::getJson( QString gamekey )
{
    QFile f;
    if ( !gamekey.isEmpty() )
        f.setFileName( saveJsonPath() + gamekey + ".json" );
    else    
        f.setFileName( saveJsonPath() + "user_order" + ".json" );

    f.open( QFile::ReadOnly );
    QByteArray data = f.readAll();
    f.close();

    if (data.isEmpty())
    {
        if ( gamekey.isEmpty() ) requestUserData( API_USER_ORDER );
        else
            requestUserData( API_ORDER, gamekey );
    }

    return QJsonDocument::fromJson(data);
}


void HumbleUserData_t::saveJson( ) const
{
    qDebug() << "Saving json orders to " << saveJsonPath() << "order for " << _user << endl;

    QDir dir( saveJsonPath() );
    if ( !dir.exists() )
        dir.mkpath( saveJsonPath() );

    int user_order_size = _user_order.array().size();
    int order_size      = _order.array().size();

    //user_order is not mandatory. We can obtain order from loadJson. But if we have user_order use it to ensure order was fetched completely.
    if ( user_order_size == 0 || user_order_size == order_size ) 
    {
        for ( auto o : _order.array() ) {
            QString gamekey = o.toObject()["gamekey"].toString();

            if ( o.toObject()["owner_username"] != _user )
                qDebug("HumbleUserData_t::saveJson: owner_username != _user");

            QFile json( saveJsonPath() + gamekey + ".json" );
            json.open(  QFile::WriteOnly );
            json.write( QJsonDocument(o.toObject()).toJson()  );
            json.close();
        }
    }
}


void HumbleUserData_t::setUser( QString user )
{   
    if ( _user != user ) {
        if ( !_user.isEmpty() )  saveJson();
        _user = user;
    }   
}


QList<QString>  HumbleUserData_t::knownUsers() const
{
    QDir dir( saveUserPath() );

    if (!dir.exists()) return QList<QString>();

    return dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot,QDir::Name );
}


void HumbleUserData_t::requestUserData( RequestType type, QString gamekey, QString machine_name )
{
    switch (type)
    {
    case API_USER_ORDER: {
        saveJson();

        _order      = QJsonDocument( QJsonArray() );
        _user_order = QJsonDocument();      

        QNetworkRequest request;
        request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_USERDATA), type );
        request.setUrl( QUrl(HumbleNetwork_t::urlApiUserOrder()) );
        HumbleNetwork_t::instance().get( request );
        break;
    }
    case API_ORDER: {
        QMap<QString,int> owners;
        for ( auto userorder : _user_order.array() ) {
            QString gamekey = userorder.toObject()["gamekey"].toString();
            QString user    = userorder.toObject()["owner_username"].toString();

            if ( owners.keys().contains(user) ) 
                owners[user] += 1;
                else
                    owners[user] =  1;

            QNetworkRequest request;
            request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_USERDATA), type );
            request.setUrl( QUrl(HumbleNetwork_t::urlApiOrder() + "/" + gamekey) );
            HumbleNetwork_t::instance().get( request );              
        }

        if ( owners.uniqueKeys().size() > 1 ) 
            qDebug("HumbleUserData_t::requestUserData: UserOrder contains mutliple owners. Deciding by frequency.");

        QString new_user;
        int max = 0;
        for ( auto guy : owners.keys() )
            if ( owners[guy] > max ) {
                new_user = guy;
                max   = owners[guy];
            }

        setUser( new_user );
        break;
    }
    case ICON_REQUEST: {
        QJsonDocument s = subproduct( gamekey, machine_name );
        QNetworkRequest request;
        request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_USERDATA), type );
        request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_GAMEKEY), gamekey);
        request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_MACHINE_NAME), machine_name);
        request.setUrl( QUrl(s.object()["icon"].toString()) );
        HumbleNetwork_t::instance().get( request );
        break;
    }
    case OTHER:
        break;
    };
}


void HumbleUserData_t::receiveData( QNetworkReply* reply )
{
    int http_status       = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    QVariant type_variant = reply->request().attribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_USERDATA) );
    RequestType type      = OTHER;
    if (type_variant.type() != QVariant::Invalid)  type = (RequestType)type_variant.toInt();

    if ( http_status == 200 )
    {
        if ( type == API_USER_ORDER || type == API_ORDER )
        {
            if ( http_status == 301 )
            {
                QJsonDocument redirect = QJsonDocument::fromJson( reply->readAll() );
                qDebug() << "API Request returned http status 301. Interpreting result as json data: [ redirect url ]";

                try {
                    QNetworkRequest request;
                    request.setUrl( QUrl(redirect.array().at(0).toString()) );
                    request.setAttribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_USERDATA),type );
                    HumbleNetwork_t::instance().get( request );
                }catch(...) {
                    qDebug() <<"HumbleUserData_t::receiveData: Reinterpreting json data as array [ url ] failed.";
                }
            }
            else {
                if ( type == API_USER_ORDER ) {
                    _user_order = QJsonDocument::fromJson( reply->readAll() );
                    requestUserData( API_ORDER );
                }
                else
                if ( type == API_ORDER ) {
                    QJsonObject object  = QJsonDocument::fromJson( reply->readAll() ).object();

                    QJsonArray array = _order.array();
                    array.append( object );
                    _order.setArray( array );
                }

//                if ( _order.array().size() == _user_order.array().size() ) {
                    /* This is a quite slow way of sorting the images. Linear runtime - ok as long as you own less than 10 billion products ;-P. 
                     * We want _order to be in the same order as _user_order for easier index-based lookup */ //FIXME: not needed
/*                    for (int i=0; i<_user_order.array().size(); ++i) {
                        QString gamekey =  _user_order.array()[i].toObject()["gamekey"].toString();
                        for (int j=0; j<_order.array().size(); ++j) {
                            if (gamekey == _order.array()[j].toObject()["gamekey"].toString()) {
                                if (i != j) { //Order was found at position j, supposed to be at i. Replace value of i with value of j.
                                    QJsonObject temp = _order.array()[i].toObject();
                                    _order.array().replace( i,_order.array()[j].toObject() );
                                    _order.array().replace( j,temp );
                                }
                            }
                        }
                    }
                }*/

                emit orderReceived( _order.array().size(), _user_order.array().size() );
            }
        }
        else 
            if( type == ICON_REQUEST )
            {
                QPixmap pix;
                pix.loadFromData( reply->readAll() );
         
                QVariant gamekey_var      = reply->request().attribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_GAMEKEY) ).toString();
                QVariant machine_name_var = reply->request().attribute( QNetworkRequest::Attribute(HumbleNetwork_t::HUMBLE_MACHINE_NAME) ).toString();
            
                if (gamekey_var.isValid() && machine_name_var.isValid())
                {
                    QString gamekey      = gamekey_var.toString();
                    QString machine_name = machine_name_var.toString();

                    saveIcon( pix,gamekey,machine_name );
                }

            }
    }
    else
        if ( http_status == 404 )
        {
            //TODO: error handling
        }
}

