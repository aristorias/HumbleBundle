
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

#ifndef HUMBLEMAINWIN_H
#define HUMBLEMAINWIN_H

#include <QtWidgets>

#include "humbleuserdata.h"

#include "widgets/gamedetailswidget.h"
#include "widgets/librarylistwidget.h"
#include "simplebrowser.h"


class HumbleMainWin_t : public QDialog
{
    Q_OBJECT

    SimpleBrowser_t*     _browser;    

    GameDetailsWidget_t* _details;
    LibraryListWidget_t* _liblist;

    QSplitter*        _details_splitter;

    QWidget*          _library;
    QSplitter*        _lib_splitter;
    
    QPushButton* _hb_button;
    QPushButton* _lib_button;
    QPushButton* _usr_button;


public:
    HumbleMainWin_t( );

public slots:   
    void navigateBrowser();
    void navigateLibrary();

    void showProductDetails( QString gamekey, QString machine_name );
};

#endif

