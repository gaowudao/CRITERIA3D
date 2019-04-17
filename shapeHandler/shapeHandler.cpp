#include "shapeHandler.h"


std::string Crit3DShapeHandler::getFilepath() const
{
    return m_filepath;
}

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

void Crit3DShapeHandler::closeDBF()
{
    if (m_dbf != nullptr)
    {
        DBFClose(m_dbf);
    }
    m_dbf = nullptr;
}

void Crit3DShapeHandler::closeSHP()
{
    if ((m_handle != nullptr))
    {
        SHPClose(m_handle);
    }
    m_handle = nullptr;
}

bool Crit3DShapeHandler::open(std::string filename)
{
    close();
    m_handle = SHPOpen(filename.c_str(), "r+b");
    m_dbf = DBFOpen(filename.c_str(), "r+b");
    m_filepath = filename;
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

bool Crit3DShapeHandler::openDBF(std::string filename)
{
    closeDBF();
    m_dbf = DBFOpen(filename.c_str(), "r+b");
    if (m_dbf == nullptr) return false;

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

bool Crit3DShapeHandler::openSHP(std::string filename)
{
    closeSHP();
    m_handle = SHPOpen(filename.c_str(), "r+b");
    if (m_handle == nullptr) return false;

    SHPGetInfo(m_handle, &m_count, &m_type, nullptr, nullptr);
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

int	Crit3DShapeHandler::getDBFRecordCount()
{
    return m_dbf->nRecords;
}

int	Crit3DShapeHandler::isDBFRecordDeleted(int record)
{
    return DBFIsRecordDeleted(m_dbf, record);
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

bool Crit3DShapeHandler::writeIntAttribute(int shapeNumber, int fieldPos, int nFieldValue)
{
    return DBFWriteIntegerAttribute(m_dbf,shapeNumber,fieldPos,nFieldValue);
}

double Crit3DShapeHandler::readDoubleAttribute(int shapeNumber, int fieldPos)
{
    return DBFReadDoubleAttribute(m_dbf,shapeNumber,fieldPos);
}

bool Crit3DShapeHandler::writeDoubleAttribute(int shapeNumber, int fieldPos, double dFieldValue)
{
    return DBFWriteDoubleAttribute(m_dbf,shapeNumber,fieldPos,dFieldValue);
}

std::string Crit3DShapeHandler::readStringAttribute(int shapeNumber, int fieldPos)
{
    return DBFReadStringAttribute(m_dbf,shapeNumber,fieldPos);
}

bool Crit3DShapeHandler::writeStringAttribute(int shapeNumber, int fieldPos, const char* pszFieldValue)
{
    return DBFWriteStringAttribute(m_dbf,shapeNumber,fieldPos, pszFieldValue);
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

bool Crit3DShapeHandler::addField(const char * fieldName, int type, int nWidth, int nDecimals )
{

    DBFFieldType DBFtype;
    if (type == 0)
    {
        DBFtype = FTString;
    }
    else if (type == 1)
    {
        DBFtype = FTInteger;
    }
    else if (type == 2)
    {
        DBFtype = FTDouble;
    }
    else
    {
        DBFtype = FTInvalid;
    }

    if (DBFAddField(m_dbf, fieldName, DBFtype, nWidth, nDecimals) != -1)
    {
        m_fields = m_fields + 1;
        m_fieldsList.push_back(std::string(fieldName));
        m_fieldsTypeList.push_back(DBFtype);
        return true;
    }
    else
    {
        return false;
    }
}

bool Crit3DShapeHandler::removeField(int iField)
{
    if (DBFDeleteField(m_dbf, iField))
    {
        m_fields = m_fields - 1;
        m_fieldsList.erase(m_fieldsList.begin()+iField);
        m_fieldsTypeList.erase(m_fieldsTypeList.begin()+iField);
        return true;
    }
    else
    {
        return false;
    }
}

/// DA TESTARE LC ///
void Crit3DShapeHandler::pack(std::string newFile)
{
    DBFHandle hDBF;
    hDBF = DBFCreate(newFile.c_str());


    // copy fields
    for( int i = 0; i < m_fields; i++ )
    {
        int nWidth = m_dbf->panFieldSize[i];
        int nDecimals = m_dbf->panFieldDecimals[i];
        DBFAddField( hDBF, m_fieldsList.at(i).c_str(), m_fieldsTypeList.at(i), nWidth, nDecimals );
    }

    //copy records if not deleted
    int nRecord = DBFGetRecordCount(m_dbf);
    int newCount = 0;
    for (int i = 0; i<nRecord; i++)
    {
        if (!DBFIsRecordDeleted(m_dbf, i))
        {
            newCount = newCount + 1;
            const char * row = DBFReadTuple(m_dbf, i );
            DBFWriteTuple(hDBF, newCount, (void*)row);
        }

    }
    DBFClose( hDBF );

}
