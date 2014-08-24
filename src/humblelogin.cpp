
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


#include "humblelogin.h"
#include "humblenetwork.h"
#include "humbleuserdata.h"

HumbleLogin_t::HumbleLogin_t(  ) : QDialog()
{
    _browser = nullptr;

    QVBoxLayout* main_layout = new QVBoxLayout( this );

    _un_label    = new QLabel( tr("Username:"),this );
    _pw_label    = new QLabel( tr("Password:"),this );
 
    _un_combo    = new QComboBox( this );
    _pw_edit     = new QLineEdit( this );

    QVBoxLayout* captcha_layout = new QVBoxLayout;

    _captcha_box      = new QGroupBox( tr("Captcha"), this );
    _captcha_label    = new QLabel( this );
    _captcha_response = new QLineEdit( this );

    _ok_button   = new QPushButton( tr("&Login"),this );
    _exit_button = new QPushButton( tr("&Cancel"),this );

    QGroupBox* issues_box  = new QGroupBox( tr("Account issues"),this );

    _register_label = new QLabel( tr("No Account?"),this );
    _register_button= new QPushButton( tr("&Register new account"),this );

    _newpw_label    = new QLabel( tr("Forgot password?"),this );
    _newpw_button   = new QPushButton( tr("Request &new password"),this );

    _weblogin_label = new QLabel( tr("Still trouble loggin in?"),this );
    _weblogin_button= new QPushButton( tr("Login via &website"),this );

    //Credentials: Login,Cancel + coresponding linedits/labels
    QGridLayout* cred_layout   = new QGridLayout();
    QHBoxLayout* button_layout = new QHBoxLayout();

    cred_layout->addWidget( _un_label,1,1 );
    cred_layout->addWidget( _pw_label,2,1 );
    cred_layout->addWidget( _un_combo,1,2 );
    cred_layout->addWidget( _pw_edit, 2,2 );

    button_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum) );
    button_layout->addWidget( _ok_button  );
    button_layout->addWidget( _exit_button);
    button_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum) );

    //Captcha
    captcha_layout->addWidget( _captcha_label );
    captcha_layout->addWidget( _captcha_response );
    _captcha_box->setLayout( captcha_layout );

    //Issues Box:
    QGridLayout* issues_layout = new QGridLayout();
    
    issues_layout->addWidget( _register_label, 1,1 );
    issues_layout->addWidget( _newpw_label   , 2,1 );
    issues_layout->addWidget( _weblogin_label, 3,1 );
    issues_layout->addWidget( _register_button,1,2 );
    issues_layout->addWidget( _newpw_button   ,2,2 );
    issues_layout->addWidget( _weblogin_button,3,2 );

    issues_box->setLayout( issues_layout );

    //Bring it all together
    main_layout->addLayout( cred_layout );
    main_layout->addWidget( _captcha_box );
    main_layout->addLayout( button_layout);
    main_layout->addWidget( issues_box  );
    main_layout->addItem( new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding) );

    setLayout( main_layout );

    //Add functionality
    _register_label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    _newpw_label->setAlignment   ( Qt::AlignRight | Qt::AlignVCenter );
    _weblogin_label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

    main_layout->setSizeConstraint( QLayout::SetFixedSize );

    _un_combo->setEditable( true );
    for ( QString user : HumbleUserData_t::instance().knownUsers() ) 
        _un_combo->addItem( user );

    _pw_edit->setEchoMode( QLineEdit::Password );

    connect( _exit_button,&QPushButton::clicked, this,&HumbleLogin_t::close );
    connect( _ok_button,  &QPushButton::clicked, [=] () {
        emit credentialsReady( username(),_pw_edit->text(),_captcha_response->text() );
//        _pw_edit->setText(""); //TODO: Do it when login was sucessful!
    });
    connect( _register_button,&QPushButton::clicked, this, &HumbleLogin_t::solveIssueRegister );
    connect( _newpw_button   ,&QPushButton::clicked, this, &HumbleLogin_t::solveIssuePassword );
    connect( _weblogin_button,&QPushButton::clicked, this, &HumbleLogin_t::solveIssueWeblogin );

    setCaptcha( QPixmap() );
}


QString HumbleLogin_t::username() const {
    return _un_combo->currentText();
}

void HumbleLogin_t::hideEvent( QHideEvent* event )
{
    if (_browser != nullptr ) _browser->hide();
    QWidget::hideEvent( event );
}

void HumbleLogin_t::solveIssueRegister()
{
    if (_browser == nullptr) _browser = new SimpleBrowser_t;
    _browser->load( QUrl(HumbleNetwork_t::urlSignup()) );
    _browser->setWindowModality( Qt::ApplicationModal );
    _browser->show();
}

void HumbleLogin_t::solveIssuePassword()
{
    if (_browser == nullptr) _browser = new SimpleBrowser_t;
    _browser->load( QUrl(HumbleNetwork_t::urlForgotPassword()) );
    _browser->setWindowModality( Qt::ApplicationModal );
    _browser->show();
}

void HumbleLogin_t::solveIssueWeblogin()
{
    if (_browser == nullptr) _browser = new SimpleBrowser_t;
    _browser->load( QUrl(HumbleNetwork_t::urlLogin()) );
    _browser->setWindowModality( Qt::ApplicationModal );
    _browser->show();
}

void HumbleLogin_t::setCaptcha( const QPixmap& captcha )
{
    _captcha_label->setPixmap( captcha );

    if (captcha.isNull()) 
        _captcha_box->hide();     
    else
            _captcha_box->show();
}

