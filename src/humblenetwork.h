
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


#ifndef HUMBLENETWORK_H
#define HUMBLENETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


class HumbleNetwork_t : public QNetworkAccessManager
{
    Q_OBJECT

    short     _loggedIn = -1;

    void setState( bool loggedIn );

public:
    //FIXME: manager for userdata id?
    static QNetworkRequest::Attribute HUMBLE_LOGIN;
    static QNetworkRequest::Attribute HUMBLE_USERDATA;
    static QNetworkRequest::Attribute HUMBLE_GAMEKEY;
    static QNetworkRequest::Attribute HUMBLE_MACHINE_NAME;
    static QNetworkRequest::Attribute CAPTCHA;
    static QNetworkRequest::Attribute HTTP_STATUS;

    //FIXME: unrelated to humblenetwork
    static QNetworkRequest::Attribute PLATFORM;
    static QNetworkRequest::Attribute DOWNLOAD_ID;  //HumbleWeb
    static QNetworkRequest::Attribute TIMESTARTED; //FileDwonloader
    static QNetworkRequest::Attribute FILEDUMP;
    static QNetworkRequest::Attribute NREDIRECTIONS;

    static QNetworkRequest::Attribute TOTAL;

    static HumbleNetwork_t& instance();

    static QString urlRoot();
    static QString urlSignup();
    static QString urlForgotPassword();
    static QString urlLogin();
    static QString urlLogout();
    static QString urlHome();
    static QString urlApi();
    static QString urlApiOrder();
    static QString urlApiUserOrder();

    void login( QString user,QString pw,QString captcha="",QString challenge="" );

public slots:
    void seekCaptcha(); //will emit captcha signal

signals:
    void captchaSought( const QPixmap&, QString challenge );
    void loginStateChanged( bool );
    void humbleRequestFinished( QNetworkReply* );

private:
    HumbleNetwork_t();

private slots:
    void requestFinished( QNetworkReply* );
};

#endif
