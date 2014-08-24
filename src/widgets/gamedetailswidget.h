
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


#ifndef GAMEDETAILSWIDGET_H
#define GAMEDETAILSWIDGET_H

#include <QtWidgets>

class GameDetailsWidget_t : public QScrollArea
{
public:
    GameDetailsWidget_t();

    void setSubproduct( const QJsonDocument&, QString platform );
    void setOrder( const QJsonDocument& );
};


extern QWidget* createSubproductWidget( const QJsonDocument& );
extern QWidget* createOrderWidget( const QJsonDocument& );

#endif
