#include "pch.h"
#include "ObjectWrapper.h"

using namespace DLEDotNet::ManagedWrappers;

LevelObject::LevelObject(CGameObject* object) :
    m_object{ object }
{
}

int LevelObject::ObjectIndex::get()
{
    return static_cast<int>(m_object->Index());
}
