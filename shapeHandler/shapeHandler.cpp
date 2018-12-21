#include "shapeHandler.h"


Crit3DShapeHandler::Crit3DShapeHandler()
    : m_handle(nullptr), m_count(0), m_type(0)
{}

Crit3DShapeHandler::~Crit3DShapeHandler()
{
    close();
}


void Crit3DShapeHandler::close()
{
    if (m_handle != nullptr)
    {
        SHPClose(m_handle);
    }
    m_handle = nullptr;
}


bool Crit3DShapeHandler::open(std::string filename)
{
    close();
    m_handle = SHPOpen(filename.c_str(), "rb");
    if (m_handle == nullptr) return false;

    SHPGetInfo(m_handle, &m_count, &m_type, nullptr, nullptr);
    return true;
}


std::string Crit3DShapeHandler::getTypeString()
{
    return getShapeTypeAsString(m_type);
}


bool Crit3DShapeHandler::getShape(int index, Crit3DShape &shape)
{
    if (m_handle == nullptr) return false;

    SHPObject *obj = SHPReadObject(m_handle, index);
    shape.assign(obj);
    SHPDestroyObject(obj);

    return true;
}


int	Crit3DShapeHandler::getShapeCount()
{
    return m_count;
}

int	Crit3DShapeHandler::getType()
{
    return m_type;
}
