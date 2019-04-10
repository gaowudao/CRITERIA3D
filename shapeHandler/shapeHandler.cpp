#include "shapeHandler.h"


Crit3DShapeHandler::Crit3DShapeHandler()
    : m_handle(nullptr), m_dbf(nullptr), m_count(0), m_type(0)
{}

Crit3DShapeHandler::~Crit3DShapeHandler()
{
    close();
}


void Crit3DShapeHandler::close()
{
    if ((m_handle != nullptr) || (m_dbf != nullptr))
    {
        SHPClose(m_handle);
        DBFClose(m_dbf);
    }
    m_handle = nullptr;
    m_dbf = nullptr;
}


bool Crit3DShapeHandler::open(std::string filename)
{
    close();
    m_handle = SHPOpen(filename.c_str(), "r+b");
    m_dbf = DBFOpen(filename.c_str(), "r+b");
    if ( (m_handle == nullptr) || (m_dbf == nullptr)) return false;

    SHPGetInfo(m_handle, &m_count, &m_type, nullptr, nullptr);
    m_fields = m_dbf->nFields;

    unsigned long size = 20;
    char *fieldName =  (char *) malloc(sizeof(char) * size);
    DBFFieldType fieldType;

    for (int i = 0; i<m_fields; i++)
    {
        fieldType = DBFGetFieldInfo( m_dbf, i, fieldName, nullptr, nullptr);
        m_fieldsList.push_back(std::string(fieldName));
        m_fieldsTypeList.push_back(fieldType);
    }
    free(fieldName);
    return true;
}


std::string Crit3DShapeHandler::getTypeString()
{
    return getShapeTypeAsString(m_type);
}


bool Crit3DShapeHandler::getShape(int index, ShapeObject &shape)
{
    if ( (m_handle == nullptr) || (m_dbf == nullptr)) return false;

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

int Crit3DShapeHandler::getFieldNumbers()
{
    return m_fields;
}

std::string	Crit3DShapeHandler::getFieldName(int fieldPos)
{
    return m_fieldsList.at(fieldPos);
}

DBFFieldType Crit3DShapeHandler::getFieldType(int fieldPos)
{
    return m_fieldsTypeList.at(fieldPos);
}

int Crit3DShapeHandler::readIntAttribute(int shapeNumber, int fieldPos)
{
    return DBFReadIntegerAttribute(m_dbf,shapeNumber,fieldPos);
}

double Crit3DShapeHandler::readDoubleAttribute(int shapeNumber, int fieldPos)
{
    return DBFReadDoubleAttribute(m_dbf,shapeNumber,fieldPos);
}

std::string Crit3DShapeHandler::readStringAttribute(int shapeNumber, int fieldPos)
{
    return DBFReadStringAttribute(m_dbf,shapeNumber,fieldPos);
}

bool Crit3DShapeHandler::deleteRecord(int shapeNumber)
{
    return DBFMarkRecordDeleted(m_dbf,shapeNumber,true);
}

bool Crit3DShapeHandler::addRecord(std::vector<std::string> fields)
{
    if( DBFGetFieldCount(m_dbf) != fields.size() )
    {
        return false;
    }

    int iRecord = DBFGetRecordCount( m_dbf );
    int typeField;

    /* -------------------------------------------------------------------- */
    /*	Loop assigning the new field values.				*/
    /* -------------------------------------------------------------------- */
    for( int i = 0; i < DBFGetFieldCount(m_dbf); i++ )
    {
        typeField = getFieldType(i);
        if( fields.at(i) == "")
        {
            DBFWriteNULLAttribute(m_dbf, iRecord, i );
        }
        else if( typeField == 0 )
        {
            DBFWriteStringAttribute(m_dbf, iRecord, i, fields.at(i).c_str() );
        }
        else if (typeField == 1)
        {
            DBFWriteIntegerAttribute(m_dbf, iRecord, i, std::stoi(fields.at(i))) ;
        }
        else if (typeField == 2)
        {
            DBFWriteDoubleAttribute(m_dbf, iRecord, i, std::stod(fields.at(i)) );
        }

    }

    return true;
}
