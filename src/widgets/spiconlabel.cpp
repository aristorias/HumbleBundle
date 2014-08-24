
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


#include "spiconlabel.h"

#include "humbleuserdata.h"

SubproductIconLabel_t::SubproductIconLabel_t( QString gamekey, QString machine_name ) : QLabel(), m_gamekey(gamekey), m_subproduct(machine_name)
{
    QSize icon_size(80,80);
    setFixedSize(icon_size);
    setPixmap( HumbleUserData_t::instance().getIcon(m_gamekey,m_subproduct) );

    connect( &HumbleUserData_t::instance(), &HumbleUserData_t::iconChanged, this, &SubproductIconLabel_t::setSubproductIcon );
}


void SubproductIconLabel_t::setSubproductIcon( QPixmap pix, QString gamekey, QString subproduct )
{
    if (gamekey == m_gamekey && subproduct == m_subproduct ) setPixmap(pix);
}
