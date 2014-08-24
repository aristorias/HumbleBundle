
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


#ifndef HUMBLELOGIN_H
#define HUMBLELOGIN_H

#include <QtWidgets>

#include "simplebrowser.h"

//TODO: delete browser when unneeded. Nicer: share it with humblemainwin somehow?

class HumbleLogin_t : public QDialog
{
    Q_OBJECT

    QLabel* _un_label;
    QLabel* _pw_label;
 
    QComboBox* _un_combo;
    QLineEdit* _pw_edit;

    QPushButton* _ok_button;   
    QPushButton* _exit_button;

    QGroupBox*   _captcha_box;
    QLabel*      _captcha_label;
    QLineEdit*   _captcha_response;

    QLabel*      _register_label;
    QPushButton* _register_button;

    QLabel*      _newpw_label;
    QPushButton* _newpw_button; 

    QLabel*      _weblogin_label;
    QPushButton* _weblogin_button;

    SimpleBrowser_t* _browser;

protected:
    void hideEvent( QHideEvent* );

public:
    HumbleLogin_t( );

    QString username() const;

public slots:
    void solveIssueRegister();
    void solveIssuePassword();
    void solveIssueWeblogin();
    void setCaptcha( const QPixmap& );

signals:
    void credentialsReady( QString username,QString password,QString captcha );
};

#endif
