#ifndef SHAPEHANDLER_H
#define SHAPEHANDLER_H

    #include <string>
    #include <vector>
    #include <shapefil.h>
    #include "shapeObject.h"

    class Crit3DShapeHandler
    {
    protected:
        SHPHandle	m_handle;
        DBFHandle   m_dbf;
        int			m_count;
        int			m_type;
        int         m_fields;
        std::vector<std::string> m_fieldsList;
        std::vector<DBFFieldType> m_fieldsTypeList;

    public:
        Crit3DShapeHandler();
        ~Crit3DShapeHandler();

        bool open(std::string filename);
        void close();
        bool getShape(int index, ShapeObject &shape);
        int	getShapeCount();
        int	getType();
        int getFieldNumbers();
        std::string	getFieldName(int fieldPos);
        DBFFieldType getFieldType(int fieldPos);
        std::string	getTypeString();
        int readIntAttribute(int shapeNumber, int fieldPos);
        double readDoubleAttribute(int shapeNumber, int fieldPos);
        std::string readStringAttribute(int shapeNumber, int fieldPos);
        bool deleteRecord(int shapeNumber);
        bool addRecord(std::vector<std::string> fields);
    };


#endif // SHAPEHANDLER_H
