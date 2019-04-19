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
        std::string m_filepath;
        std::vector<std::string> m_fieldsList;
        std::vector<DBFFieldType> m_fieldsTypeList;

    public:
        Crit3DShapeHandler();
        ~Crit3DShapeHandler();

        bool open(std::string filename);
        bool openDBF(std::string filename);
        bool openSHP(std::string filename);
        void close();
        void closeDBF();
        void closeSHP();
        bool getShape(int index, ShapeObject &shape);
        int	getShapeCount();
        int	getDBFRecordCount();
        int	isDBFRecordDeleted(int record);
        int	getType();
        int getFieldNumbers();
        std::string	getFieldName(int fieldPos);
        DBFFieldType getFieldType(int fieldPos);
        std::string	getTypeString();
        int readIntAttribute(int shapeNumber, int fieldPos);
        bool writeIntAttribute(int shapeNumber, int fieldPos, int nFieldValue);
        double readDoubleAttribute(int shapeNumber, int fieldPos);
        bool writeDoubleAttribute(int shapeNumber, int fieldPos, double dFieldValue);
        std::string readStringAttribute(int shapeNumber, int fieldPos);
        bool writeStringAttribute(int shapeNumber, int fieldPos, const char* pszFieldValue);
        bool deleteRecord(int shapeNumber);
        //bool addRecord(std::vector<std::string> fields);
        bool addField(const char * fieldName, int type, int nWidth, int nDecimals );
        bool removeField(int iField);
        std::string getFilepath() const;
        void packDBF(std::string newFile);
        void packSHP(std::string newFile);
    };


#endif // SHAPEHANDLER_H
