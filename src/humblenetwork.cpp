
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


#include <QtWebKitWidgets>
#include <QNetworkRequest>

#include "humblenetwork.h"

QNetworkRequest::Attribute HumbleNetwork_t::HUMBLE_LOGIN        = QNetworkRequest::Attribute(QNetworkRequest::User+0);
QNetworkRequest::Attribute HumbleNetwork_t::HUMBLE_USERDATA     = QNetworkRequest::Attribute(QNetworkRequest::User+1);
QNetworkRequest::Attribute HumbleNetwork_t::HUMBLE_GAMEKEY      = QNetworkRequest::Attribute(QNetworkRequest::User+2);
QNetworkRequest::Attribute HumbleNetwork_t::HUMBLE_MACHINE_NAME = QNetworkRequest::Attribute(QNetworkRequest::User+3);
QNetworkRequest::Attribute HumbleNetwork_t::CAPTCHA      = QNetworkRequest::Attribute(QNetworkRequest::User+4);
QNetworkRequest::Attribute HumbleNetwork_t::DOWNLOAD_ID  = QNetworkRequest::Attribute(QNetworkRequest::User+5);
QNetworkRequest::Attribute HumbleNetwork_t::HTTP_STATUS  = QNetworkRequest::Attribute(QNetworkRequest::User+6);

QNetworkRequest::Attribute HumbleNetwork_t::TIMESTARTED    = QNetworkRequest::Attribute(QNetworkRequest::User+7);
QNetworkRequest::Attribute HumbleNetwork_t::FILEDUMP       = QNetworkRequest::Attribute(QNetworkRequest::User+8);
QNetworkRequest::Attribute HumbleNetwork_t::PLATFORM       = QNetworkRequest::Attribute(QNetworkRequest::User+9);
QNetworkRequest::Attribute HumbleNetwork_t::NREDIRECTIONS  = QNetworkRequest::Attribute(QNetworkRequest::User+10);

QNetworkRequest::Attribute HumbleNetwork_t::TOTAL   = QNetworkRequest::Attribute(QNetworkRequest::User+10);

HumbleNetwork_t& HumbleNetwork_t::instance()
{
    static HumbleNetwork_t network;
    return network;
}

HumbleNetwork_t::HumbleNetwork_t() : QNetworkAccessManager()
{
    connect( this, &QNetworkAccessManager::finished, this, &HumbleNetwork_t::requestFinished );
}

void HumbleNetwork_t::setState( bool state )
{
    if (state != _loggedIn) {
        _loggedIn = state;

        emit loginStateChanged( state );
    }
}

void HumbleNetwork_t::seekCaptcha( )
{
    QWebPage* webpage = new QWebPage;
  
    connect( webpage, &QWebPage::loadFinished, [=] ( bool ok ) 
    {
        Q_UNUSED( ok );
        qDebug() << "HumbleNetwork_t::seekCaptcha: Loading " << urlLogin() << " in background. Seeking captcha" << ok;

        QWebElement frame = webpage->mainFrame()->documentElement().findFirst( "div#recaptcha_image" ).findFirst("img");
    
        if ( !frame.isNull() ) 
        {
            qDebug("HumbleNetwork_t::seekCaptcha: Captcha found");
            QString captcha_url = frame.attribute("src");

            QNetworkRequest request;
            request.setAttribute( QNetworkRequest::Attribute(CAPTCHA), true );
            request.setUrl( QUrl(captcha_url) );
            get( request );
        }
        else {
            qDebug("HumbleNetwork_t::seekCaptcha: Captcha NOT found");
            emit captchaSought( QPixmap(),"" );
        }
    });

    webpage->mainFrame()->load( urlLogin() );
}


void HumbleNetwork_t::requestFinished( QNetworkReply* reply )
{
    //1: We will be redirected to login-page if logged out and home-page if logged-in
    //2: Implement auto-redirect

    int http_status = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    bool is_captcha = reply->request().attribute( QNetworkRequest::Attribute(CAPTCHA) ).isValid();

    if ( is_captcha ) {
        QPixmap captcha;
        captcha.loadFromData( reply->readAll() );

        try{
            QString challenge = reply->request().url().toDisplayString();
            QStringList sep   = challenge.split("=");
            challenge         = sep.at(1);
            emit captchaSought( captcha,challenge );
        }
        catch (...) {
            qDebug() << "HumbleNetwork_t::requestFinished: Failed extracting captcha challenge from url" << reply->request().url().toDisplayString();
            emit captchaSought( QPixmap(),"" );
        }   

    }
    else
        if ( http_status == 302 || http_status == 301 ) //Redirect
        {
            QVariant redirect_variant = reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
            if (redirect_variant.type() != QVariant::Invalid)
            {
                QUrl target = redirect_variant.toUrl();
                QNetworkRequest request( reply->request() );
                request.setUrl( target );
                get( request );
            }
            
        }
        else
        {
            //If we end up at login page or logout page change the state (false)
            //Or if we expect home-page after ::login was called change the state (false)
            //Seeing our home-page (aka. library) we know we're in.
            if ( reply->url().fileName() == QUrl(urlHome()).fileName() ) {
                setState(true);
            }
            else 
                if ( reply->url().fileName() == QUrl(urlLogin()).fileName() || reply->url().fileName() == QUrl(urlLogout()).fileName() )  {
                    setState(false);
                }
                else
                    if ( reply->attribute( QNetworkRequest::Attribute(HUMBLE_LOGIN) ).toBool() ) {
                        setState(false);
                    }
            

            emit humbleRequestFinished( reply );
        }
}

void HumbleNetwork_t::login( QString username,QString password,QString captcha,QString challenge )
{
    _loggedIn = -1;

    QByteArray postdata;
    postdata.append( "username=" );
    postdata.append( username );
    postdata.append( "&password=" );
    postdata.append( password );

    if (!captcha.isNull()) {
        postdata.append( "&recaptcha_challenge_field=" );
        postdata.append( challenge );
        postdata.append( "&recaptcha_response_field=" );
        postdata.append( captcha );
    }

    QNetworkRequest request;
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
    request.setAttribute( QNetworkRequest::Attribute(HUMBLE_LOGIN), true );
    request.setUrl( QUrl(urlLogin()) );
    post( request,postdata );
}


QString HumbleNetwork_t::urlRoot(){
    return QString("https://www.humblebundle.com");
}

QString HumbleNetwork_t::urlSignup(){
    return urlRoot() + "/signup";
}

QString HumbleNetwork_t::urlForgotPassword(){
    return urlRoot() + "/user/forgot";
}

QString HumbleNetwork_t::urlLogin(){
    return urlRoot() + "/login?goto=/home";
}

QString HumbleNetwork_t::urlLogout(){
    return urlRoot() + "/logout?goto=/";
}

QString HumbleNetwork_t::urlHome(){
    return urlRoot() + "/home";
}

QString HumbleNetwork_t::urlApi(){
    return urlRoot() + "/api/v1";
}

QString HumbleNetwork_t::urlApiOrder(){
    return urlApi() + "/order";
}

QString HumbleNetwork_t::urlApiUserOrder(){
    return urlApi() + "/user/order";
}
