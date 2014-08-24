
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

#ifndef LIBRARYWIDGET_H
#define LIBRARYWIFGET_H

#include <QtWidgets>

class LibraryListWidget_t : public QWidget
{
    Q_OBJECT

    QTreeWidget* _lib_tree;
    QLineEdit*   _lib_search;
    QComboBox*   _lib_filter;

    QStringList  _custom_filters;

    QPushButton* _view_products;   //Humble Bundle 6 Toplevel item
    QPushButton* _view_categories; //Store, weekly, bundle Toplevel items
    
    enum ListItemData {
        GAMEKEY = Qt::UserRole,
        MACHINE_NAME
    };

public:
    LibraryListWidget_t();

    void    addCustomFilter( const QString& );
    QString currentFilter( ) const;
    QString filter( int ) const;

    void setIcon( const QPixmap&, const QString& gamekey, const QString& machine_name );

public slots:
    void setSearchString( const QString& );
    void setFilter( int id );
    void updateLibrary( QString platform = "all" );
    void activateItem( QTreeWidgetItem* item, int col );

signals:
    void currentFilterChanged( QString filter );
    void productSelected( QString gamekey, QString machine_name );
};

#endif
