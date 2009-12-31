#ifndef TXML_HPP_INCLUDED
#define TXML_HPP_INCLUDED

#include <boost/shared_ptr.hpp>

#include "tinyxml.h"

namespace txml
{
    typedef ::TiXmlDocument document;
    typedef ::TiXmlElement element;
    typedef ::TiXmlNode node;
    typedef ::TiXmlBase base;

    typedef boost::shared_ptr<document> documentPtr;
}

#endif // TXML_HPP_INCLUDED
