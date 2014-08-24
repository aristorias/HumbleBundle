#include "pficonlabel.h"

PlatformIconLabel_t::PlatformIconLabel_t( QString service, QStringList platforms )
{
    setPixmap ( QString(":/platforms/%1.png").arg(service) );
    setToolTip( platforms.join("\n") );
}


