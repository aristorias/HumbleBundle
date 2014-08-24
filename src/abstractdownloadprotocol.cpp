
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


#include <QList>

#include <limits>

#include "abstractdownloadprotocol.h"

QList<dlid_t> AbstractDownloadProtocol::m_used_ids = QList<dlid_t>();

AbstractDownloadProtocol::AbstractDownloadProtocol()
{
}


AbstractDownloadProtocol::~AbstractDownloadProtocol()
{
}

dlid_t AbstractDownloadProtocol::takeId()
{
    dlid_t id = 0;

    for (dlid_t i=1; i<std::numeric_limits<dlid_t>::max(); ++i)
    {
        if ( m_used_ids.size() < int(i) )
        {
            m_used_ids.push_back( i );  
            id = i;
            break;
        }
        else
        {
            if ( i != m_used_ids[i-1] )
            {
                m_used_ids.insert( i-1,i );
                id = i;
                break;
            }
        }
    }

    return id;
}


void AbstractDownloadProtocol::releaseId( dlid_t id ) {
    m_used_ids.removeOne( id );
}

