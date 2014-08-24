
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


#include <QtWidgets>

#include <functional>

#include "humbleuserdata.h"
#include "librarylistwidget.h"

LibraryListWidget_t::LibraryListWidget_t( ) : QWidget()
{
    //Allocate widgets
    QVBoxLayout* main_layout = new QVBoxLayout;
 
    _lib_search = new QLineEdit;
    _lib_filter = new QComboBox;
    _lib_tree   = new QTreeWidget;

    _view_products   = new QPushButton;
    _view_categories = new QPushButton;
    
    QHBoxLayout* view_layout = new QHBoxLayout;

    //Configure widgets
    _lib_tree->header()->close();

    _lib_filter->setFrame( false );

    _lib_search->setPlaceholderText( tr("Search...") );
    _lib_search->setClearButtonEnabled( true );

    QHBoxLayout* search_filter_layout = new QHBoxLayout;
    search_filter_layout->setContentsMargins( 0,0,0,0 );
    search_filter_layout->setSpacing( 0 );

    
    //Bring it all togehter
    search_filter_layout->addWidget( _lib_search );
    search_filter_layout->addWidget( _lib_filter );

    view_layout->addWidget( _view_products );
    view_layout->addWidget( _view_categories );

    main_layout->addLayout( search_filter_layout );
    main_layout->addWidget( _lib_tree );
    main_layout->addLayout( view_layout );
    
    setLayout( main_layout );

    //Add functionlity
    connect( _lib_search, &QLineEdit::textChanged, this, &LibraryListWidget_t::setSearchString );
    connect( _lib_tree, &QTreeWidget::itemClicked, this, &LibraryListWidget_t::activateItem );
    connect( _lib_tree, &QTreeWidget::itemActivated, this, &LibraryListWidget_t::activateItem );
    connect( _lib_tree, &QTreeWidget::itemDoubleClicked, this, &LibraryListWidget_t::activateItem );
}


void LibraryListWidget_t::setSearchString( const QString& search )
{
    const QList<QTreeWidgetItem*> visible = _lib_tree->findItems( search,Qt::MatchRecursive|Qt::MatchContains );

    std::function< bool(QTreeWidgetItem*) > hide_or_not_hide;
    hide_or_not_hide = [&visible,&hide_or_not_hide] ( QTreeWidgetItem* item ) -> bool 
    {
        bool visible_child = false;
        for ( int i=0; i<item->childCount(); ++i )
            visible_child = hide_or_not_hide( item->child(i) ) || visible_child;

        item->setHidden( !visible.contains(item) && !visible_child );
        return !item->isHidden() || visible_child;
    };

    hide_or_not_hide( _lib_tree->invisibleRootItem() );
}


void LibraryListWidget_t::addCustomFilter( const QString& filter ) {
    //Will add a Separator and custom filters in platform filter combo box (call updateLibrary)
    _custom_filters.append( filter );
}


void LibraryListWidget_t::setFilter( int i )
{
    QString platform = filter(i);
    
    if (!platform.isEmpty())
    {
        updateLibrary( platform ); //FIXME: TOO SLOW? item->setData( platform ). set Visible yes or not: Use searchString? Or QTimer::singleShot?
        emit currentFilterChanged( platform );
    }
}


QString LibraryListWidget_t::currentFilter() const
{
    return filter( _lib_filter->currentIndex() );
}

QString LibraryListWidget_t::filter( int i ) const
{
    QString platform;
    QVariant platform_var = _lib_filter->itemData( i );

    if ( platform_var.isValid() ) platform = platform_var.toString();
    return platform;
}


void LibraryListWidget_t::updateLibrary( QString platform )
{
    //Remember settings
    int old_index =  _lib_filter->currentIndex();

    //Recreate tree
    _lib_filter->disconnect( SIGNAL(currentIndexChanged(int)), this, SLOT(setFilter(int)) );

    _lib_tree->clear();
    _lib_filter->clear();

    QMap< QString,int > platforms;
    platforms["all"] = 0;

    for ( auto o : HumbleUserData_t::instance().order().array() ) {
        QString product = o.toObject()["product"].toObject()["human_name"].toString();
        QString gamekey = o.toObject()["gamekey"].toString();

        QTreeWidgetItem* product_item = new QTreeWidgetItem( QStringList(product) );
        product_item->setData( 0, GAMEKEY, gamekey );

        for ( auto sp : o.toObject()["subproducts"].toArray() ) {
            QString subproduct   = sp.toObject()["human_name"].toString();
            QString machine_name = sp.toObject()["machine_name"].toString();

            QSet<QString> subproduct_platforms;
            for ( auto dl : sp.toObject()["downloads"].toArray() ) {
                QString platform = dl.toObject()["platform"].toString();
                subproduct_platforms.insert(platform);
            }

            for ( auto platform : subproduct_platforms ) 
                if ( !platforms.contains(platform) ) platforms[platform] = 1;
                else {
                    platforms["all"]    += 1;
                    platforms[platform] += 1;
                }

            if ( platform.isNull() || subproduct_platforms.contains(platform) || platform == "all" ) {
                QTreeWidgetItem* subproduct_item = new QTreeWidgetItem( product_item, QStringList(subproduct) );
                subproduct_item->setIcon( 0,QIcon(HumbleUserData_t::instance().getIcon(gamekey,machine_name)) );
                subproduct_item->setData( 0, GAMEKEY,      gamekey );
                subproduct_item->setData( 0, MACHINE_NAME, machine_name );
            }
  
        }
    
        if ( product_item->childCount() == 0 ) product_item->setHidden( true );

        _lib_tree->addTopLevelItem( product_item );
    }


    //Set available platform filters
    for ( auto platform : platforms.keys() ) 
        _lib_filter->addItem( QIcon(QString(":/platforms/%1.png").arg(platform)), QString("%1 (%2)").arg(platform).arg(platforms.value(platform)),QVariant(platform) ); 
   
    if ( _custom_filters.count() > 0 )
        _lib_filter->insertSeparator( _lib_filter->count() );

    for ( auto custom : _custom_filters )
        _lib_filter->addItem( QString(custom) );

    //Restore old settings: Platform filter, Opened and closed TopLevelItems ...
    _lib_filter->setCurrentIndex( old_index );

    connect( _lib_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilter(int)) );
}


void LibraryListWidget_t::setIcon( const QPixmap& icon, const QString& gamekey, const QString& machine_name )
{
    QTreeWidgetItemIterator it( _lib_tree );
    while (*it ) {
        QTreeWidgetItem* item = *it;

        QVariant gamekey_var      = item->data( 0, GAMEKEY );
        QVariant machine_name_var = item->data( 0, MACHINE_NAME );
    
        if ( gamekey_var.isValid() && machine_name_var.isValid() ) {
            QString item_gamekey      = gamekey_var.toString();
            QString item_machine_name = machine_name_var.toString();

            if ( item_gamekey == gamekey && item_machine_name == machine_name )
                item->setIcon( 0,QIcon(icon) );
        }

        ++it;
    }
}


void LibraryListWidget_t::activateItem( QTreeWidgetItem* item, int col )
{
    Q_UNUSED(col);
    
    QString gamekey      = item->data( 0, GAMEKEY ).toString();
    QString machine_name = item->data( 0, MACHINE_NAME ).toString();
 
    emit productSelected( gamekey,machine_name );
}

