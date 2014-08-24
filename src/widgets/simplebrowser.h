
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


#ifndef SIMPLEBROWSER_H
#define SIMPLEBROWSER_H

#include <QtWidgets/QWidget>
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebPage>

class SimpleBrowser_t : public QWidget
{
    QWebView* _webview;
    QWebPage* _webpage;

public:
    SimpleBrowser_t( );

    void load( QUrl url );
};

#endif
